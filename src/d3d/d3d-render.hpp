#pragma once

#include <ctet/ctet.h>

#include "d3d11.h"
#include "d3d-game.hpp"

struct Renderer {
    IDXGISwapChain *swapChain;
    ID3D11Device *device;
    ID3D11DeviceContext *deviceContext;
    ID3D11RenderTargetView *renderTarget;
    
};

void renderer_init(Renderer *renderer, HWND window, int width, int height);

void renderer_cleanup(Renderer *renderer);

void renderer_drawFrame(Renderer *renderer, CTetEngine *engine, GameRenderingContext *context);