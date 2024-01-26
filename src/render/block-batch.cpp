#include "block-batch.hpp"

#include "../math/vector2-cross-ctet.hpp"

static void stageActive(const CTetEngine *engine, BlockBatch *batch) {
    const auto active = ctEngine_getActivePiece(engine);
    const auto activePos = vector2_addToNew(
        vector2_fromCtPoint(ctEngine_getActivePiecePos(engine)),
        {-(CT_FIELD_WIDTH / 2), -(CT_VISIBLE_FIELD_HEIGHT / 2)}
        );
    for (int i = 0; i < CT_BLOCKS_PER_PIECE; i++) {
        auto position = vector2_fromCtPoint(active->coords[i]);
        vector2_add(&position, activePos);
        batch->blockInstances[batch->len].position = position;
        batch->len++;
    }
}

static void stageField(const CTetEngine *engine, BlockBatch *batch) {
    for (int y = 0; y < CT_TOTAL_FIELD_HEIGHT; y++) {
        for (int x = 0; x < CT_FIELD_WIDTH; x++) {
            const auto block = ctEngine_getBlockAtFieldLocation(engine, {x, y});
            if (block->color == CTetBlockColor_NONE) {
                continue;
            }
            batch->blockInstances[batch->len].position =
                vector2_addToNew(
                    {static_cast<float>(x), static_cast<float>(y)},
                    {-(CT_FIELD_WIDTH / 2), -(CT_VISIBLE_FIELD_HEIGHT / 2)}
                    );
            batch->len++;
        }
    }
}

void blockBatch_stageAll(const CTetEngine *engine, BlockBatch *batch) {
    batch->len = 0;
    stageActive(engine, batch);
    stageField(engine, batch);
}


void blockBatch_init(BlockBatch *batch) {
    batch->len = 0;
}
