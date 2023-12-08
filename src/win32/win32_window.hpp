#pragma once

#include <windows.h>
#include <ctet/ctet.h>
#include "../control/control.hpp"
#include "../render/d3d11/d3d11_renderer.hpp"

struct Win32Window {
    HINSTANCE instance;
    LPCWSTR className;
    HWND window;
    D3d11Renderer d3d11Renderer;
    ControlTracker controlTracker;
};

void win32Window_init(Win32Window *window, HINSTANCE instance);

void win32Window_loop(Win32Window *window, CTetEngine *engine);

void win32Window_show(HWND window);