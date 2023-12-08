#pragma once

#include <ctet/ctet.h>

#include "d3d11.h"
#include "d3d11_engine-rendering-context.hpp"

struct D3d11Renderer {
    IDXGISwapChain *swapChain;
    ID3D11Device *device;
    ID3D11DeviceContext *deviceContext;
    ID3D11RenderTargetView *renderTarget;
    ID3D11Buffer *aspectRatioBuffer;
    AspectConstantBuffer aspectRatioBufferData;
};

void d3d11Renderer_init(D3d11Renderer *renderer, HWND window, int width, int height);


void d3d11Renderer_drawFrame(D3d11Renderer *renderer, CTetEngine *engine, D3d11EngineRenderingCtx *context);

void d3d11_createRenderTargetView(
        IDXGISwapChain *swapChain,
        ID3D11Device *device,
        ID3D11DeviceContext *deviceContext,
        ID3D11RenderTargetView **target
);

void d3d11_setViewport(int width, int height, ID3D11DeviceContext *deviceContext);

void d3d11_createBuffer(ID3D11Device *device, const void *initData, ID3D11Buffer **destBuffer, D3D11_BUFFER_DESC bufferDesc);

void d3d11Renderer_setAspectRatio(D3d11Renderer *renderer);

void d3d11Renderer_resize(D3d11Renderer *renderer, int width, int height, bool isMinimized);