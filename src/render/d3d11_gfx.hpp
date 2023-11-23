#pragma once

#include "gfx.hpp"
#include <windows.h>

struct D3d11Gfx;

D3d11Gfx *d3d11gfx_win32_create(const GfxFunctions **destFns, HWND window, int width, int height);