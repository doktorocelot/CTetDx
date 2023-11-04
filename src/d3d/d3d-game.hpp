#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "shader-pair.hpp"

extern "C" {
#include <ctet/engine.h>
}

static constexpr int PIECE_BLOCK_COUNT = 4;

struct BlockVertex {
    DirectX::XMFLOAT3 position;
};

struct BlockGroup {
    BlockVertex vertices[PIECE_BLOCK_COUNT];
};

struct BlockBatch {
    BlockGroup activePiece[PIECE_BLOCK_COUNT];
    BlockGroup nextPieces[NEXT_QUEUE_LENGTH][PIECE_BLOCK_COUNT];
    BlockGroup holdPiece[PIECE_BLOCK_COUNT];
    BlockGroup field[FIELD_HEIGHT][FIELD_WIDTH];
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