#include <cstdlib>
#include <sstream>
#include "win32_window.hpp"
#include "win32_check-result.hpp"
#include "win32_kill-program.hpp"
#include "../util/fps-counter.hpp"

#include <iostream>
#include <iomanip>

#include "win32_memory.hpp"

#define WINDOW_TITLE L"CTetDx"
#define WINDOW_WIDTH  720
#define WINDOW_HEIGHT 720
#define CTET_WINDOW_PROP_NAME L"CTetDx"

static void resizeWindow(const WPARAM wparam, const LPARAM lparam, Win32Window *window) {
    const bool isMinimized = wparam != SIZE_MINIMIZED;
    d3d11Renderer_resize(&window->d3d11Renderer, LOWORD(lparam), HIWORD(lparam), isMinimized);
}

static void setWindowCentered(HWND windowHandle, const MONITORINFO&mi) {
    RECT windowRect;
    windowRect.left = (mi.rcMonitor.right - mi.rcMonitor.left - WINDOW_WIDTH) / 2 + mi.rcMonitor.left;
    windowRect.top = (mi.rcMonitor.bottom - mi.rcMonitor.top - WINDOW_HEIGHT) / 2 + mi.rcMonitor.top;
    windowRect.right = windowRect.left + WINDOW_WIDTH;
    windowRect.bottom = windowRect.top + WINDOW_HEIGHT;

    SetWindowLongPtr(windowHandle, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
    SetWindowPos(windowHandle, nullptr, windowRect.left, windowRect.top,
                 windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_FRAMECHANGED);
}

static void toggleFullscreen(HWND windowHandle) {
    MONITORINFO mi = {sizeof(mi)};
    HMONITOR hMonitor = MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST);
    GetMonitorInfo(hMonitor, &mi);

    if (GetWindowLongPtr(windowHandle, GWL_STYLE) & WS_POPUP) {
        // Switch to windowed mode
        setWindowCentered(windowHandle, mi);
    }
    else {
        // Switch to fullscreen mode
        SetWindowLongPtr(windowHandle, GWL_STYLE, WS_VISIBLE | WS_POPUP);
        SetWindowPos(windowHandle, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                     mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_FRAMECHANGED);
    }
}

LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto *window = static_cast<Win32Window *>(GetProp(windowHandle, CTET_WINDOW_PROP_NAME));
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(windowHandle);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN: {
            if (GetAsyncKeyState('Q') & 0x8000) {
                PostMessage(windowHandle, WM_CLOSE, 0, 0);
            }
            if (GetAsyncKeyState('F') & 0x8000) {
                toggleFullscreen(windowHandle);
            }
            controlTracker_updateCurrent(&window->controlTracker);
            break;
        }
        case WM_KEYUP:
            controlTracker_updateCurrent(&window->controlTracker);
            break;
        case WM_MOUSEMOVE:
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            break;
        case WM_SIZE:
            resizeWindow(wparam, lparam, window);
            break;
        default:
            return DefWindowProc(windowHandle, msg, wparam, lparam);
    }
    return 0;
}

void win32Window_init(Win32Window *window, HINSTANCE instance) {
    window->className = L"MainWindowClass";

    WNDCLASSEX wc = {
        .cbSize = sizeof(WNDCLASSEX),
        .lpfnWndProc = windowProcedure,
        .hInstance = instance,
        .hbrBackground = (HBRUSH)(COLOR_BACKGROUND),
        .lpszClassName = window->className,
    };
    RegisterClassEx(&wc);

    window->instance = instance;

    window->window = CreateWindowEx(
        0,
        window->className,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        nullptr, nullptr, instance, nullptr
    );

    if (window->window == nullptr) {
        win32_killProgram(L"Window could not be created.");
    }

    SetProp(window->window, CTET_WINDOW_PROP_NAME, window);
    RECT windowClient{};
    GetClientRect(window->window, &windowClient);
    d3d11Renderer_init(&window->d3d11Renderer, window->window,
        windowClient.right - windowClient.left,
        windowClient.bottom - windowClient.top);
}

static FpsCounter *initializeFpsCounter(size_t historyLength) {
    auto counter = static_cast<FpsCounter *>(win32_allocateMemory(fpsCounter_getSize(historyLength)));
    fpsCounter_init(counter, historyLength);
    return counter;
}

void win32Window_loop(Win32Window *window, CTetEngine *engine) {
    MSG msg;
    LARGE_INTEGER frequency, lastTime, currentTime;
    float deltaTime;
    CTetMessage ctMsg;
    bool gameIsPlaying = true;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    ControlTracker *controlTracker = &window->controlTracker;
    controlTracker->keyAssign[VK_LEFT] = Control_SHIFT_LEFT;
    controlTracker->keyAssign[VK_RIGHT] = Control_SHIFT_RIGHT;
    controlTracker->keyAssign[static_cast<int>('Z')] = Control_ROTATE_LEFT;
    controlTracker->keyAssign[static_cast<int>('X')] = Control_ROTATE_RIGHT;
    controlTracker->keyAssign[VK_UP] = Control_HARD_DROP;
    controlTracker->keyAssign[VK_DOWN] = Control_SOFT_DROP;
    controlTracker->keyAssign[static_cast<int>('C')] = Control_HOLD;
    controlTracker->keyAssign[static_cast<int>('R')] = Control_RETRY;

    auto fpsCounterUpdate = initializeFpsCounter(200);
    auto fpsCounterDraw = initializeFpsCounter(50);
    
    std::locale locale("");

    D3d11EngineRenderingCtx ctx {};
    d3d11EngineRenderingCtx_init(&ctx, window->d3d11Renderer.device, window->d3d11Renderer.aspectRatioBuffer);

    constexpr float TIME_BETWEEN_FPS_UPDATES = 0.5;
    float timeSinceLastFpsUpdate = TIME_BETWEEN_FPS_UPDATES;

    constexpr float TIME_BETWEEN_RENDERS = 1.0f / 240;
    float timeSinceLastRender = 0;

    Text texts[16] = {};
    texts[0] = {
        .string = "NEXT",
        .position = {CT_FIELD_WIDTH / 2 + 1,
                     CT_VISIBLE_FIELD_HEIGHT / 2 + 0.5},
        .size = 1.2,
        .alignment = TextAlignment_LEFT,
    };
    texts[1] = {
        .string = "HOLD",
        .position = {-(CT_FIELD_WIDTH / 2 + 1),
                     CT_VISIBLE_FIELD_HEIGHT / 2 + 0.5},
        .size = 1.2,
        .alignment = TextAlignment_RIGHT,
    };
    
    texts[2] = {
        .string = "Lines",
        .position = {-(CT_FIELD_WIDTH / 2 + 1),
                     -3},
        .size = 1,
        .alignment = TextAlignment_RIGHT,
    };

    constexpr int LINES_TEXT_BYTES = 6;
    char linesText[LINES_TEXT_BYTES] = {};
    texts[3] = {
        .string = linesText,
        .position = {-(CT_FIELD_WIDTH / 2 + 1),
                     -4.5},
        .size = 1.5,
        .alignment = TextAlignment_RIGHT,
    };

    texts[4] = {
        .string = "Pieces",
        .position = {-(CT_FIELD_WIDTH / 2 + 1),
                     -6},
        .size = 1,
        .alignment = TextAlignment_RIGHT,
    };

    constexpr int PIECES_TEXT_BYTES = 6;
    char piecesText[PIECES_TEXT_BYTES] = {};
    texts[5] = {
        .string = piecesText,
        .position = {-(CT_FIELD_WIDTH / 2 + 1),
                     -7.5},
        .size = 1.5,
        .alignment = TextAlignment_RIGHT,
    };

    texts[6] = {
        .string = "Level",
        .position = {-(CT_FIELD_WIDTH / 2 + 1),
                     -9},
        .size = 1,
        .alignment = TextAlignment_RIGHT,
    };

    constexpr int LEVEL_TEXT_BYTES = 6;
    char levelText[LEVEL_TEXT_BYTES] = {};
    texts[7] = {
        .string = levelText,
        .position = {-(CT_FIELD_WIDTH / 2 + 1),
                     -10.5},
        .size = 1.5,
        .alignment = TextAlignment_RIGHT,
    };
    
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                // We are done. Nuke the program into orbit.
                return;
            }
        }

        QueryPerformanceCounter(&currentTime);

        deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / (float)frequency.QuadPart;

        fpsCounter_pushFrameTime(fpsCounterUpdate, deltaTime);

        timeSinceLastFpsUpdate += deltaTime;
        if (timeSinceLastFpsUpdate >= TIME_BETWEEN_FPS_UPDATES) {
            std::wstringstream ss;
            ss.imbue(locale);
            ss
                    << WINDOW_TITLE
                    << L";    Updates/Sec: " << std::fixed << std::setprecision(0)
                    << fpsCounter_getFps(fpsCounterUpdate)
                    << L", FrameTime Millis: " << std::fixed << std::setprecision(7)
                    << fpsCounter_getAverageFrameTime(fpsCounterUpdate) * 1000
                    << L";    FPS: " << std::fixed << std::setprecision(0) << fpsCounter_getFps(fpsCounterDraw);
            timeSinceLastFpsUpdate -= TIME_BETWEEN_FPS_UPDATES;
            SetWindowText(window->window, ss.str().c_str());
        }

        if (gameIsPlaying) {
            ctEngine_update(engine, deltaTime * 1000);

            if (keyPressed(controlTracker, Control_SHIFT_LEFT)) ctEngine_onShiftLeftDown(engine);
            if (keyReleased(controlTracker, Control_SHIFT_LEFT)) ctEngine_onShiftLeftUp(engine);

            if (keyPressed(controlTracker, Control_SHIFT_RIGHT)) ctEngine_onShiftRightDown(engine);
            if (keyReleased(controlTracker, Control_SHIFT_RIGHT)) ctEngine_onShiftRightUp(engine);

            if (keyPressed(controlTracker, Control_ROTATE_LEFT)) ctEngine_onRotateLeft(engine);
            if (keyPressed(controlTracker, Control_ROTATE_RIGHT)) ctEngine_onRotateRight(engine);

            if (keyPressed(controlTracker, Control_HARD_DROP)) ctEngine_onHardDrop(engine);

            if (keyPressed(controlTracker, Control_SOFT_DROP)) ctEngine_onSoftDropDown(engine);
            if (keyReleased(controlTracker, Control_SOFT_DROP)) ctEngine_onSoftDropUp(engine);

            if (keyPressed(controlTracker, Control_HOLD)) ctEngine_onHoldDown(engine);
        }

        if (keyPressed(controlTracker, Control_RETRY)) {
            ctEngine_reset(engine);
            gameIsPlaying = true;
        }

        while (ctMsg = ctEngine_nextMessage(engine), ctMsg.id != CT_MSG_NONE) {
            switch (ctMsg.id) {
                case CT_MSG_GAME_OVER:
                    gameIsPlaying = false;
                    break;
                default:
                    break;
            }
        }

        lastTime = currentTime;

        controlTracker_copyCurrentToPrev(controlTracker);

        timeSinceLastRender += deltaTime;
        if (timeSinceLastRender >= TIME_BETWEEN_RENDERS) {
            fpsCounter_pushFrameTime(fpsCounterDraw, timeSinceLastRender);
            timeSinceLastRender -= TIME_BETWEEN_RENDERS;

            const CTetStats ctEngineGetStats = *ctEngine_getStats(engine);
            snprintf(linesText, LINES_TEXT_BYTES, "%d", ctEngineGetStats.lines);
            snprintf(piecesText, LINES_TEXT_BYTES, "%d", ctEngineGetStats.pieces);
            snprintf(levelText, LINES_TEXT_BYTES, "%d", ctEngineGetStats.level);
            textRenderer_setText(&ctx.textRenderer, texts, sizeof(texts) / sizeof(Text));

            d3d11Renderer_drawFrame(&window->d3d11Renderer, engine, &ctx);
        }
    }
}

void win32Window_show(HWND window) {
    MONITORINFO mi = {sizeof(mi)};
    HMONITOR hMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    GetMonitorInfo(hMonitor, &mi);
    setWindowCentered(window, mi);
    ShowWindow(window, SW_SHOWNORMAL);
    UpdateWindow(window);
}
