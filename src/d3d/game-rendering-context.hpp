#pragma once
#include "d3d11.h"
#include "block-batch.hpp"
#include "mesh.hpp"

struct FrameVertex {
    Vector3 position;
};

struct alignas(16) AspectConstantBuffer {
    float aspectRatio;
};

struct GameRenderingContext {
    BlockBatch blockBatch;
    Mesh blockMesh;
    Mesh frameMesh;
};

void gameRenderingContext_init(GameRenderingContext *ctx, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer);
void gameRenderingContext_cleanup(GameRenderingContext *ctx);
void updateBlockBatch(BlockBatch *batch, Mesh *mesh, CTetEngine *engine, ID3D11DeviceContext *deviceContext);
