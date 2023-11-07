#pragma once

#include <DirectXMath.h>
extern "C"{
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
