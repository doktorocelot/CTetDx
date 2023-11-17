#include <cstdlib>
#include <sstream>
#include "window.hpp"
#include "../die.hpp"

#include <iostream>

#define WINDOW_TITLE L"CTet"
#define WINDOW_WIDTH  720
#define WINDOW_HEIGHT 720
#define CTET_WINDOW_PROP_NAME L"CTetDx"


static void resizeWindow(WPARAM wparam, LPARAM lparam, Window *window) {
    Renderer *renderer = &window->renderer;
    if (renderer->device != nullptr && wparam != SIZE_MINIMIZED) {
        if (renderer->renderTarget != nullptr) {
            renderer->renderTarget->Release();
            renderer->renderTarget = nullptr;
        }

        if (renderer->swapChain != nullptr) {
            WORD width = LOWORD(lparam);
            WORD height = HIWORD(lparam);
            renderer->swapChain->ResizeBuffers(
                    1,
                    width,
                    height,
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    0
            );
            
            renderer->aspectRatioBufferData = {(float) width / (float) height};
            renderer_setAspectRatio(renderer);

            createRenderTargetView(renderer->swapChain, renderer->device, renderer->deviceContext, &renderer->renderTarget);
            setViewport(width, height, renderer->deviceContext);
        }
    }
}

static void setWindowCentered(HWND windowHandle, const MONITORINFO &mi) {
    RECT windowRect;
    windowRect.left = (mi.rcMonitor.right - mi.rcMonitor.left - WINDOW_WIDTH) / 2 + mi.rcMonitor.left;
    windowRect.top = (mi.rcMonitor.bottom - mi.rcMonitor.top - WINDOW_HEIGHT) / 2 + mi.rcMonitor.top;
    windowRect.right = windowRect.left + WINDOW_WIDTH;
    windowRect.bottom = windowRect.top + WINDOW_HEIGHT;

    SetWindowLongPtr(windowHandle, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
    SetWindowPos(windowHandle, nullptr, windowRect.left, windowRect.top,
                 windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_FRAMECHANGED);
}

void toggleFullscreen(HWND windowHandle) {
    MONITORINFO mi = { sizeof(mi) };
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
    auto *window = static_cast<Window *>(GetProp(windowHandle, CTET_WINDOW_PROP_NAME));
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

static void unregisterClassFromWindow(Window *window) {
    UnregisterClass(window->className, window->instance);
}

void window_init(Window *window, HINSTANCE instance) {
    window->className = L"MainWindowClass";

    WNDCLASSEX wc = {
            .cbSize = sizeof(WNDCLASSEX),
            .lpfnWndProc = windowProcedure,
            .hInstance = instance,
            .hbrBackground = (HBRUSH) (COLOR_BACKGROUND),
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
        unregisterClassFromWindow(window);
        die("Window could not be created.");
    }

    SetProp(window->window, CTET_WINDOW_PROP_NAME, window);

    renderer_init(&window->renderer, window->window, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void window_loop(Window *window, CTetEngine *engine) {
    MSG msg;
    LARGE_INTEGER frequency, lastTime, currentTime;
    float deltaTime;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    GameRenderingContext ctx = {};
    gameRenderingContext_init(&ctx, window->renderer.device, window->renderer.aspectRatioBuffer);
    ControlTracker *controlTracker = &window->controlTracker;
    controlTracker->keyAssign[VK_LEFT] = Control_SHIFT_LEFT;
    controlTracker->keyAssign[VK_RIGHT] = Control_SHIFT_RIGHT;
    controlTracker->keyAssign[static_cast<int>('Z')] = Control_ROTATE_LEFT;
    controlTracker->keyAssign[static_cast<int>('X')] = Control_ROTATE_RIGHT;
    controlTracker->keyAssign[VK_UP] = Control_HARD_DROP;
    controlTracker->keyAssign[VK_DOWN] = Control_SOFT_DROP;
    controlTracker->keyAssign[static_cast<int>('C')] = Control_HOLD;
    controlTracker->keyAssign[static_cast<int>('R')] = Control_RETRY;
    
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                gameRenderingContext_cleanup(&ctx);
                return;
            }
        }

        QueryPerformanceCounter(&currentTime);

        deltaTime = (float) (currentTime.QuadPart - lastTime.QuadPart) / (float) frequency.QuadPart;

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

        if (keyPressed(controlTracker, Control_RETRY)) ctEngine_reset(engine);
        if (keyPressed(controlTracker, Control_HOLD)) ctEngine_onHoldDown(engine);

        lastTime = currentTime;

        controlTracker_copyCurrentToPrev(controlTracker);

        renderer_drawFrame(&window->renderer, engine, &ctx);
    }

}

void window_show(HWND window) {
    MONITORINFO mi = { sizeof(mi) };
    HMONITOR hMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    GetMonitorInfo(hMonitor, &mi);
    setWindowCentered(window, mi);
    ShowWindow(window, SW_SHOWNORMAL);
    UpdateWindow(window);
}

void window_cleanup(Window *window) {
    renderer_cleanup(&window->renderer);
    unregisterClassFromWindow(window);
    DestroyWindow(window->window);
}
