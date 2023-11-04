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
        exit(-1); // Todo proper error
    }

    renderer_init(&window->renderer, window->window, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void window_loop(Window *window) {
    MSG msg;
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                return;
            }
        }

        renderer_drawFrame(&window->renderer);
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
