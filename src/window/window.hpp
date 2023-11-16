#pragma once

#include <windows.h>
#include <ctet/ctet.h>
#include "../d3d/d3d-render.hpp"
#include "../control/control.hpp"

struct Window {
    HINSTANCE instance;
    LPCWSTR className;
    HWND window;
    Renderer renderer;
    ControlTracker controlTracker;
};

void window_init(Window *window, HINSTANCE instance);

void window_loop(Window *window, CTetEngine *engine);

void window_show(HWND window);

void window_cleanup(Window *window);