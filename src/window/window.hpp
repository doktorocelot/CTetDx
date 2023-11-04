#pragma once

#include <windows.h>
#include "../d3d/d3d-render.hpp"
extern "C" {
#include "ctet/engine.h"
}

struct Window {
    HINSTANCE instance;
    LPCSTR className;
    HWND window;
    Renderer renderer;
};

void window_init(Window *window, HINSTANCE instance);

void window_loop(Window *window, Engine *engine);

void window_show(HWND window);

void window_cleanup(Window *window);