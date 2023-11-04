#include <cstdlib>
#include "window.hpp"

#define WINDOW_TITLE "CTet"
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

LRESULT CALLBACK windowProcedure(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(window);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
            if (wparam == 'q' || wparam == 'Q') {
                PostMessage(window, WM_CLOSE, 0, 0);
            }
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
        exit(-1); // Todo proper error
    }
}

void window_loop(HWND window) {
    MSG msg;
    BOOL messageReturn;
    while ((messageReturn = GetMessage(&msg, window, 0, 0)) != 0) {
        if (messageReturn == -1) {
            exit(-1); // Todo proper error
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
}

void window_show(HWND window) {
    ShowWindow(window, SW_SHOWNORMAL);
    UpdateWindow(window);
}

void window_cleanup(Window *window) {
    unregisterClassFromWindow(window);
    DestroyWindow(window->window);
}
