#include <cstdlib>
#include <sstream>
#include "window.hpp"
#include "../die.hpp"
#include "../d3d/d3d-game.hpp"
#include "../control/control.hpp"

#define WINDOW_TITLE "CTet"
#define SCREEN_WIDTH  720
#define SCREEN_HEIGHT 720

LRESULT CALLBACK windowProcedure(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(window);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
            break;
        case WM_MOUSEMOVE:
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            break;
        default:
            return DefWindowProc(window, msg, wparam, lparam);
    }
    return 0;
}

static void unregisterClassFromWindow(Window *window) {
    UnregisterClass(window->className, window->instance);
}

void window_init(Window *window, HINSTANCE instance) {
    window->className = "MainWindowClass";

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
            CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT,
            nullptr, nullptr, instance, nullptr
    );

    if (window->window == nullptr) {
        unregisterClassFromWindow(window);
        die("Window could not be created.");
    }

    renderer_init(&window->renderer, window->window, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void window_loop(Window *window, Engine *engine) {
    MSG msg;
    LARGE_INTEGER frequency, lastTime, currentTime;
    float deltaTime;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);
    
    GameRenderingContext ctx{};
    gameRenderingContext_init(&ctx, window->renderer.device);
    
    ControlTracker controlTracker{};
    controlTracker.keyAssign[VK_LEFT] = Control_SHIFT_LEFT;
    controlTracker.keyAssign[VK_RIGHT] = Control_SHIFT_RIGHT;
    controlTracker.keyAssign[static_cast<int>('Z')] = Control_ROTATE_LEFT;
    controlTracker.keyAssign[static_cast<int>('X')] = Control_ROTATE_RIGHT;
    controlTracker.keyAssign[VK_UP] = Control_HARD_DROP;
    controlTracker.keyAssign[VK_DOWN] = Control_SOFT_DROP;
    controlTracker.keyAssign[static_cast<int>('C')] = Control_HOLD;
    controlTracker.keyAssign[static_cast<int>('R')] = Control_RETRY;
    
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                gameRenderingContext_cleanup(&ctx);
                return;
            }
        }
        controlTracker_updateCurrent(&controlTracker);
        short state = GetAsyncKeyState('Q');
        
        if (state & 0x8000) {
            PostMessage(window->window, WM_CLOSE, 0, 0);
        }

        QueryPerformanceCounter(&currentTime);
        
        deltaTime = (float) (currentTime.QuadPart - lastTime.QuadPart) / (float) frequency.QuadPart;
        
        engine_tick(engine, deltaTime * 1000);
        
        if (keyPressed(&controlTracker, Control_SHIFT_LEFT)) engine_onShiftLeftDown(engine); 
        if (keyReleased(&controlTracker, Control_SHIFT_LEFT)) engine_onShiftLeftUp(engine); 
        
        if (keyPressed(&controlTracker, Control_SHIFT_RIGHT)) engine_onShiftRightDown(engine); 
        if (keyReleased(&controlTracker, Control_SHIFT_RIGHT)) engine_onShiftRightUp(engine); 
        
        if (keyPressed(&controlTracker, Control_ROTATE_LEFT)) engine_onRotateLeft(engine); 
        if (keyPressed(&controlTracker, Control_ROTATE_RIGHT)) engine_onRotateRight(engine);

        if (keyPressed(&controlTracker, Control_HARD_DROP)) engine_onHardDrop(engine);
        
        if (keyPressed(&controlTracker, Control_SOFT_DROP)) engine_onSoftDropDown(engine);
        if (keyReleased(&controlTracker, Control_SOFT_DROP)) engine_onSoftDropUp(engine);
        
        if (keyPressed(&controlTracker, Control_RETRY)) engine_reset(engine);
        if (keyPressed(&controlTracker, Control_HOLD)) engine_onHoldDown(engine);

        lastTime = currentTime;

        controlTracker_copyCurrentToPrev(&controlTracker);

        renderer_drawFrame(&window->renderer, engine, &ctx);
    }
    
}

void window_show(HWND window) {
    ShowWindow(window, SW_SHOWNORMAL);
    UpdateWindow(window);
}

void window_cleanup(Window *window) {
    renderer_cleanup(&window->renderer);
    unregisterClassFromWindow(window);
    DestroyWindow(window->window);
}
