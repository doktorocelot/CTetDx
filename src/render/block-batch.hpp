#pragma once

#include <ctet/ctet.h>
#include "../math/vector2.hpp"
#include "../math/vector3.hpp"

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

void blockBatch_stageAll(const CTetEngine *engine, BlockBatch *batch);

void blockBatch_init(BlockBatch *batch);
