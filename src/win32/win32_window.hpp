#pragma once

#include <windows.h>
#include <ctet/ctet.h>
#include "../control/control.hpp"
#include "../render/gfx.hpp"

struct Win32Window {
    HINSTANCE instance;
    LPCWSTR className;
    HWND window;
    Gfx *gfx;
    const GfxFunctions *gfxFns;
    ControlTracker controlTracker;
};

void win32Window_init(Win32Window *window, HINSTANCE instance);

void win32Window_loop(Win32Window *window, CTetEngine *engine);

void win32Window_show(HWND window);

void win32Window_cleanup(Win32Window *window);