#pragma once

#include <windows.h>
#include <ctet/ctet.h>
#include "../d3d/d3d-render.hpp"

struct Window {
    HINSTANCE instance;
    LPCSTR className;
    HWND window;
    Renderer renderer;
};

void window_init(Window *window, HINSTANCE instance);

void window_loop(Window *window, CTetEngine *engine);

void window_show(HWND window);

void window_cleanup(Window *window);