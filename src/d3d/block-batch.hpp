#pragma once

#include <ctet/ctet.h>
#include "mesh.hpp"
#include "../math/math.hpp"

static constexpr int PIECE_BLOCK_COUNT = 4;
static constexpr int BLOCK_VERTEX_COUNT = 4;

struct BlockVertex {
    Vector2 position;
    float brightness;
    bool enabled;
    Vector3 color;
};

struct BlockGroup {
    BlockVertex vertices[BLOCK_VERTEX_COUNT];
};

struct BlockBatch {
    BlockGroup field[CT_TOTAL_FIELD_HEIGHT][CT_FIELD_WIDTH];
    BlockGroup nextPieces[CT_NEXT_QUEUE_MAX_LENGTH][PIECE_BLOCK_COUNT];
    BlockGroup holdPiece[PIECE_BLOCK_COUNT];
};

void blockBatch_setupActive(const CTetEngine *engine, BlockBatch *batch);

void blockBatch_setupNext(const CTetEngine *engine, BlockBatch *batch);

void blockBatch_setupHold(const CTetEngine *engine, BlockBatch *batch);

void blockBatch_setupField(const CTetEngine *engine, BlockBatch *batch);

void blockBatch_initFieldPositions(BlockBatch *batch);

void blockBatch_initNextEnabled(BlockBatch *batch);

void createBlockBatchMesh(BlockBatch *blockBatch, Mesh *blockMesh, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer);
