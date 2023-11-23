#pragma once
#include "ctet/ctet.h"

typedef void Gfx;

struct GfxFunctions {
    void (*resize)(void *gfx, int width, int height, bool isMinimized);
    void (*drawFrame)(void *gfx, CTetEngine *engine);
    void (*cleanup)(void *gfx);
};