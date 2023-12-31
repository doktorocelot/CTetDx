#pragma once
#include "d3d11.h"
#include "../block-batch.hpp"
#include "d3d11_mesh.hpp"
#include "d3d11_texture.hpp"

struct FrameVertex {
    Vector3 position;
};

struct alignas(16) AspectConstantBuffer {
    float aspectRatio;
};

struct D3d11EngineRenderingCtx {
    BlockBatch blockBatch;
    D3d11Mesh blockMesh;
    D3d11Mesh frameMesh;
    D3d11Texture blockSkinTexture;
};

void d3d11EngineRenderingCtx_init(D3d11EngineRenderingCtx *ctx, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer);
void d3d11EngineRenderingCtx_cleanup(D3d11EngineRenderingCtx *ctx);
void updateBlockBatchInMesh(BlockBatch *batch, D3d11Mesh *mesh, CTetEngine *engine, ID3D11DeviceContext *deviceContext);
