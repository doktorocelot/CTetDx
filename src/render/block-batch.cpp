#include "block-batch.hpp"

#include "../math/two-numbers.hpp"
#include "../math/vector2-cross-ctet.hpp"

static void addBlock(BlockBatch *batch, const BlockInstance instance) {
    batch->blockInstances[batch->len] = instance;
    batch->len++;
}

constexpr Vector2 TEXCOORD_OFFSET_LUT[] = {
    {0, 0},
    {0, 0},
    {1, 0},
    {2, 0},
    {3, 0},
    {0, -1},
    {1, -1},
    {2, -1},
};
constexpr Vector2 TEXCOORD_LOCKED_HOLD_OFFSET = {0, -3};
constexpr Vector2 TEXCOORD_GHOST_OFFSET = {1, -2};
static void stageActive(const CTetEngine *engine, BlockBatch *batch) {
    const auto active = ctEngine_getActivePiece(engine);
    const auto activePos = vector2_addToNew(
        vector2_fromCtPoint(ctEngine_getActivePiecePos(engine)),
        {-(CT_FIELD_WIDTH / 2), -(CT_VISIBLE_FIELD_HEIGHT / 2)}
        );
    
    //ghost
    const auto ghostPos = vector2_addToNew(activePos, vector2_fromCtPoint(ctEngine_getGhostOffset(engine)));
    for (int i = 0; i < CT_BLOCKS_PER_PIECE; i++) {
        auto position = vector2_fromCtPoint(active->coords[i]);
        vector2_add(&position, ghostPos);
        addBlock(batch, {position, 0.4f, TEXCOORD_GHOST_OFFSET});
    }

    //active
    constexpr float ratio = 0.7f;
    const float brightness = 1 - (1 - ctEngine_getLockDelayRemainingPercentage(engine)) * ratio;
    for (int i = 0; i < CT_BLOCKS_PER_PIECE; i++) {
        auto position = vector2_fromCtPoint(active->coords[i]);
        vector2_add(&position, activePos);
        addBlock(batch, {position, brightness, TEXCOORD_OFFSET_LUT[active->blocks[i].color]});
    }
}

static constexpr int LOCK_FLASH_TIMER = 35;

float calculateBrightness(double currentTimestamp, double lockedTimestamp) {
    constexpr float dimDuration = 250.0f;
    constexpr float maxBrightness = 1.5f;
    constexpr float minBrightness = 0.8f;

    const float elapsedSeconds = currentTimestamp - lockedTimestamp;

    // Ensure brightness doesn't go below the minimum
    const float brightness = max(minBrightness, maxBrightness - (elapsedSeconds / dimDuration) * (maxBrightness - minBrightness));

    return brightness;
}

static void stageField(const CTetEngine *engine, BlockBatch *batch) {
    for (int y = 0; y < CT_TOTAL_FIELD_HEIGHT; y++) {
        for (int x = 0; x < CT_FIELD_WIDTH; x++) {
            const auto block = ctEngine_getBlockAtFieldLocation(engine, {x, y});
            if (block->color == CTetBlockColor_NONE) {
                continue;
            }
            const float brightness = ctEngine_getTimestamp(engine) - block->lockedTimestamp < LOCK_FLASH_TIMER ? 999999.0f : calculateBrightness(ctEngine_getTimestamp(engine), block->lockedTimestamp);
            const auto position = vector2_addToNew(
                {static_cast<float>(x), static_cast<float>(y)},
                {-(CT_FIELD_WIDTH / 2), -(CT_VISIBLE_FIELD_HEIGHT / 2)}
                );
            addBlock(batch, {position, brightness, TEXCOORD_OFFSET_LUT[block->color]});
        }
    }
}

static Vector2 getPieceQueueOffset(const CTetPieceType type) {
    Vector2 additionalOffset{};
    switch (type) {
    case CTetPieceType_O:
        additionalOffset = {1, 1};
        break;
    case CTetPieceType_I:
        additionalOffset = {0, -0.5};
        break;
    default:
        additionalOffset = {0.5, 0};
        break;
    }
    return additionalOffset;
}

static void stageNext(const CTetEngine *engine, BlockBatch *batch) {
    CTetPoint nextOffset = {6, 7};
    CTetPoint constexpr nextAdvance = {0, -3};
    const CTetPiece *nextPieces = ctEngine_getNextPieces(engine);
    for (int i = 0; i < CT_NEXT_QUEUE_MAX_LENGTH; i++) {
        const auto piece = nextPieces[i];
        const auto nextPieceOffset = getPieceQueueOffset(piece.type);
        for (int j = 0; j < PIECE_BLOCK_COUNT; j++) {
            
            auto coords = vector2_fromCtPoint(ctPoint_addToNew(nextOffset, piece.coords[j]));
            vector2_add(&coords, nextPieceOffset);

            addBlock(batch, {coords, 1.0f, TEXCOORD_OFFSET_LUT[piece.blocks[j].color]});
        }
        ctPoint_add(&nextOffset, nextAdvance);
    }
}

static void stageHold(const CTetEngine *engine, BlockBatch *batch) {
    constexpr CTetPoint holdOffset = {-10, 7};
    const CTetPiece heldPiece = *ctEngine_getHeldPiece(engine);
    if (heldPiece.type == CTetPieceType_NONE) {
        return;
    }
    const auto offset = getPieceQueueOffset(heldPiece.type);
    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        const auto coords = vector2_fromCtPoint(ctPoint_addToNew(holdOffset, heldPiece.coords[i]));
        const auto position = vector2_addToNew(offset, coords);;
        const auto texOffset = ctEngine_holdIsLocked(engine) ? TEXCOORD_LOCKED_HOLD_OFFSET : TEXCOORD_OFFSET_LUT[heldPiece.blocks[i].color];
        addBlock(batch, {position, 1.0f, texOffset});
    }
}

void blockBatch_stageAll(const CTetEngine *engine, BlockBatch *batch) {
    batch->len = 0;
    stageActive(engine, batch);
    stageNext(engine, batch);
    stageHold(engine, batch);
    stageField(engine, batch);
}


void blockBatch_init(BlockBatch *batch) {
    batch->len = 0;
}
