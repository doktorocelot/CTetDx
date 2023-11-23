#include "d3d11_gfx.hpp"

#include "d3d11/d3d11_renderer.hpp"

struct D3d11Gfx {
    D3d11Renderer renderer;
    D3d11EngineRenderingCtx ctx;
};

void d3d11Gfx_resize(void *gfx, const int width, const int height, const bool isMinimized) {
    const auto d3d11Gfx = static_cast<D3d11Gfx *>(gfx);
    d3d11Renderer_resize(&d3d11Gfx->renderer, width, height, isMinimized);
}

void d3d11Gfx_drawFrame(void *gfx, CTetEngine *engine) {
    const auto d3d11Gfx = static_cast<D3d11Gfx *>(gfx);
    d3d11Renderer_drawFrame(&d3d11Gfx->renderer, engine, &d3d11Gfx->ctx);
}

void d3d11Gfx_cleanup(void *gfx) {
    const auto d3d11Gfx = static_cast<D3d11Gfx *>(gfx);
    d3d11EngineRenderingCtx_cleanup(&d3d11Gfx->ctx);
    d3d11Renderer_cleanup(&d3d11Gfx->renderer);
    free(d3d11Gfx);
}

constexpr GfxFunctions D3D11_DISPATCH_TABLE = {
    .resize = d3d11Gfx_resize,
    .drawFrame = d3d11Gfx_drawFrame,
    .cleanup = d3d11Gfx_cleanup,
};

D3d11Gfx *d3d11gfx_win32_create(const GfxFunctions **destFns, const HWND window, const int width, const int height) {
    const auto gfx = static_cast<D3d11Gfx *>(malloc(sizeof(D3d11Gfx)));
    *gfx = {};
    d3d11Renderer_init(&gfx->renderer, window, width, height);

    d3d11EngineRenderingCtx_init(&gfx->ctx, gfx->renderer.device, gfx->renderer.aspectRatioBuffer);
    *destFns = &D3D11_DISPATCH_TABLE;
    return gfx;
}
