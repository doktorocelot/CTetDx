#pragma once

#include <DirectXMath.h>
#include <ctet/ctet.h>

static constexpr int PIECE_BLOCK_COUNT = 4;
static constexpr int BLOCK_VERTEX_COUNT = 4;

struct BlockVertex {
    DirectX::XMFLOAT3 position;
    float brightness;
    bool enabled;
    DirectX::XMFLOAT3 color;
};

struct BlockGroup {
    BlockVertex vertices[BLOCK_VERTEX_COUNT];
};

struct BlockBatch {
    BlockGroup field[CT_TOTAL_FIELD_HEIGHT][CT_FIELD_WIDTH];
    BlockGroup nextPieces[CT_NEXT_QUEUE_MAX_LENGTH][PIECE_BLOCK_COUNT];
    BlockGroup holdPiece[PIECE_BLOCK_COUNT];
};

void blockBatch_setupActive(CTetEngine *engine, BlockBatch *batch);

void blockBatch_setupNext(CTetEngine *engine, BlockBatch *batch);

void blockBatch_setupHold(CTetEngine *engine, BlockBatch *batch);

void blockBatch_setupField(CTetEngine *engine, BlockBatch *batch);

void blockBatch_initFieldPositions(BlockBatch *batch);

void blockBatch_initNextEnabled(BlockBatch *batch);

