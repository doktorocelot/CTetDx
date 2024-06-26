#include <cstdlib>
#include <sstream>
#include "../math/simple-math.hpp"
#include "win32_window.hpp"
#include "win32_check-result.hpp"
#include "win32_files.hpp"
#include "win32_kill-program.hpp"
#include "../util/fps-counter.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>
#include "win32_memory.hpp"
#include "../audio/win32/win32_sound-bank.hpp"
#include "../audio/sound-pool.hpp"
#include "../audio/wav.hpp"
#include "../audio/wasapi/wasapi_audio_system.hpp"
#include "../audio/win32/win32_audio.hpp"

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
    double deltaTime;
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

    // Audio
    WasapiAudioSystem audioSystem = {};
    wasapiAudio_init(&audioSystem);
    
    PcmF32Buffer sounds[Sounds_LENGTH];
    win32_loadSounds(sounds);
    SoundPool soundPool = {};
    
    const float TIME_BETWEEN_AUDIO_UPDATES = static_cast<float>(AUDIO_SAMPLES_PER_UPDATE) / audioSystem.sampleRate;
    float timeSinceLastAudioUpdate = 0;

    constexpr float TIME_BETWEEN_FPS_UPDATES = 0.5;
    float timeSinceLastFpsUpdate = TIME_BETWEEN_FPS_UPDATES;

    constexpr float TIME_BETWEEN_RENDERS = 1.0f / 240;
    float timeSinceLastRender = TIME_BETWEEN_RENDERS;
    
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

        deltaTime = (double)(currentTime.QuadPart - lastTime.QuadPart) / (double)frequency.QuadPart;

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
            case CT_MSG_LOCKDOWN:
                soundPool_add(&soundPool, &sounds[Sounds_LOCK]);
                break;
            case CT_MSG_HARD_DROP:
                soundPool_add(&soundPool, &sounds[Sounds_HARD_DROP]);
                break;
            case CT_MSG_SHIFT:
                soundPool_add(&soundPool, &sounds[Sounds_SHIFT]);
                break;
            case CT_MSG_ROTATE:
                soundPool_add(&soundPool, &sounds[Sounds_ROTATE]);
                break;
            case CT_MSG_HOLD:
                soundPool_add(&soundPool, &sounds[Sounds_HOLD]);
                break;
            case CT_MSG_CLEAR_LINE: {
                constexpr Sounds SOUNDS[] = {
                    Sounds_ERASE1, // should not be here
                    Sounds_ERASE1,
                    Sounds_ERASE2,
                    Sounds_ERASE3,
                    Sounds_ERASE4
                };
                soundPool_add(&soundPool, &sounds[SOUNDS[ctMsg.detailA]]);
                break;
            }
            case CT_MSG_COLLAPSE:
                soundPool_add(&soundPool, &sounds[Sounds_COLLAPSE]);
                break;
            case CT_MSG_BUFFERED_ROTATE:
                soundPool_add(&soundPool, &sounds[Sounds_BUFFER_ROTATE]);
                break;
            case CT_MSG_BUFFERED_HOLD:
                soundPool_add(&soundPool, &sounds[Sounds_BUFFER_HOLD]);
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

            ingameText_update(&ctx.ingameText, ctEngine_getStats(engine));
            textRenderer_clearText(&ctx.textRenderer);
            textRenderer_stageText(&ctx.textRenderer, ctx.ingameText.texts, INGAME_TEXT_LEN);

            d3d11Renderer_drawFrame(&window->d3d11Renderer, engine, &ctx);
        }


        
        // Audio
        timeSinceLastAudioUpdate += deltaTime;
        while (timeSinceLastAudioUpdate >= TIME_BETWEEN_AUDIO_UPDATES) {
            timeSinceLastAudioUpdate -= TIME_BETWEEN_AUDIO_UPDATES;
            wasapiAudio_process(&audioSystem, &soundPool);
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
