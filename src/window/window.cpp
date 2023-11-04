#include <cstdlib>
#include <sstream>
#include "window.hpp"
#include "../die.hpp"
#include "../d3d/d3d-game.hpp"

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
    
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                return;
            }
        }
        
        short state = GetAsyncKeyState('Q');
        
        if (state & 0x8000) {
            PostMessage(window->window, WM_CLOSE, 0, 0);
        }

        QueryPerformanceCounter(&currentTime);
        
        deltaTime = (float) (currentTime.QuadPart - lastTime.QuadPart) / (float) frequency.QuadPart;
        
        engine_tick(engine, deltaTime * 1000);
        
        lastTime = currentTime;

        renderer_drawFrame(&window->renderer, engine, &ctx);
    }

    gameRenderingContext_cleanup(&ctx);
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
