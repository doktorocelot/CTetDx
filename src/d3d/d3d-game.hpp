#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "shader-pair.hpp"

extern "C" {
#include <ctet/engine.h>
}

struct BlockVertex {
    DirectX::XMFLOAT3 position;
};

struct BlockGroup {
    BlockVertex vertices[4];
};

struct BlockBatch {
    BlockGroup activePiece[4];
};

struct Mesh {
    ID3D11Buffer *vertexBuffer;
    ID3D11Buffer *indexBuffer;
    ShaderPair shaders;
    UINT stride;
    UINT indices;
    
};

struct GameRenderingContext {
    BlockBatch blockBatch;
    Mesh blockMesh;
};

void gameRenderingContext_init(GameRenderingContext *ctx, ID3D11Device *device);
void gameRenderingContext_cleanup(GameRenderingContext *ctx);

void updateBlockBatch(BlockBatch *batch, Mesh *mesh, Engine *engine, ID3D11DeviceContext *deviceContext);

void mesh_use(Mesh *mesh, ID3D11DeviceContext *deviceContext);
void mesh_draw(Mesh *mesh, ID3D11DeviceContext *deviceContext);

void mesh_cleanup(Mesh *mesh);