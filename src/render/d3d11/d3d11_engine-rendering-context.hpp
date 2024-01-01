#pragma once
#include "d3d11.h"
#include "../block-batch.hpp"
#include "d3d11_mesh.hpp"
#include "d3d11_texture.hpp"
#include "../text.hpp"

struct alignas(16) AspectConstantBuffer {
    float aspectRatio;
};

struct D3d11EngineRenderingCtx {
    BlockBatch blockBatch;
    D3d11Mesh blockMesh;
    D3d11Mesh frameMesh;
    D3d11Texture blockSkinTexture;
    D3d11Texture fontTexture;
    TextRenderer textRenderer;
    D3d11Mesh textMesh;
    ID3D11BlendState *textBlendState;
};

void d3d11EngineRenderingCtx_init(D3d11EngineRenderingCtx *ctx, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer);
void d3d11EngineRenderingCtx_cleanup(D3d11EngineRenderingCtx *ctx);
void updateBlockBatchInMesh(BlockBatch *batch, D3d11Mesh *mesh, CTetEngine *engine, ID3D11DeviceContext *deviceContext);
void updateTextMesh(const TextRenderer *textRenderer, D3d11Mesh *mesh, ID3D11DeviceContext *deviceContext);