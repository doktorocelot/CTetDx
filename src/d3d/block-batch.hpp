#pragma once

#include <DirectXMath.h>
extern "C"{
#include <ctet/engine.h>
}

static constexpr int PIECE_BLOCK_COUNT = 4;

struct BlockVertex {
    DirectX::XMFLOAT3 position;
    float brightness;
};

struct BlockGroup {
    BlockVertex vertices[PIECE_BLOCK_COUNT];
};

struct BlockBatch {
    BlockGroup field[FIELD_HEIGHT][FIELD_WIDTH];
    BlockGroup ghostPiece[PIECE_BLOCK_COUNT];
    BlockGroup activePiece[PIECE_BLOCK_COUNT];
    BlockGroup nextPieces[NEXT_QUEUE_LENGTH][PIECE_BLOCK_COUNT];
    BlockGroup holdPiece[PIECE_BLOCK_COUNT];
};

void blockBatch_setupActive(Engine *engine, BlockBatch *batch);

void blockBatch_setupNext(Engine *engine, BlockBatch *batch);

void blockBatch_setupHold(Engine *engine, BlockBatch *batch);

void blockBatch_setupField(Engine *engine, BlockBatch *batch);