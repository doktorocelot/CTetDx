#pragma once

#include <windows.h>

struct Window {
    HINSTANCE instance;
    LPCSTR className;
    HWND window;
};

void window_init(Window *window, HINSTANCE instance);

void window_loop(HWND window);

void window_show(HWND window);

void window_cleanup(Window *window);