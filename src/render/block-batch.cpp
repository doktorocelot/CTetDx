#include "block-batch.hpp"

#include "../math/rect.hpp"
#include "..\math\vector2-cross-ctet.hpp"

static constexpr Vector3 LOCK_FLASH_COLOR = {1, 1, 1};
static constexpr Vector3 HOLD_LOCKED_COLOR = {0.5, 0.5, 0.5};

static Vector3 colors[]{
        {0, 0, 0},
        {0.93, 0.16, 0.22},
        {1, 0.47, 0},
        {1, 0.79, 0},
        {0.41, 0.75, 0.16},
        {0, 0.62, 0.85},
        {0, 0.39, 0.74},
        {0.59, 0.18, 0.6},
};

static void setBlockVertices(BlockGroup *group, const Vector2 position) {
    group->vertices[0].position = position;
    group->vertices[1].position = vector2_addToNew(position, {1, 0});
    group->vertices[2].position = vector2_addToNew(position, {0, 1});
    group->vertices[3].position = vector2_addToNew(position, {1, 1});
}

static RectAbsolute texCoordsColorLut[]{
    {0, 0, 0, 0},
    {0.806640625, 0.99609375, 0.00390625, 0.193359375},
    {0.806640625, 0.99609375, 0.19921875, 0.388671875},
    {0.806640625, 0.99609375, 0.39453125, 0.583984375},
    {0.806640625, 0.99609375, 0.58984375, 0.779296875},
    {0.611328125, 0.80078125, 0.00390625, 0.193359375},
    {0.611328125, 0.80078125, 0.19921875, 0.388671875},
    {0.611328125, 0.80078125, 0.39453125, 0.583984375},
};

static void setBlockColor(BlockGroup *group, const Vector3 colorValue) {

}

static void setBlockColorWithCTetColor(BlockGroup *group, const CTetBlockColor color) {
    const RectAbsolute rect = texCoordsColorLut[color];
    group->vertices[0].texCoords = {rect.left, rect.top};
    group->vertices[1].texCoords = {rect.right, rect.top};
    group->vertices[2].texCoords = {rect.left, rect.bottom};
    group->vertices[3].texCoords = {rect.right, rect.bottom};
}

static void setBlockBrightness(BlockGroup *group, const float brightness) {
    for (int i = 0; i < BLOCK_VERTEX_COUNT; i++) {
        group->vertices[i].brightness = brightness;
    }
}

static void setBlockEnabled(BlockGroup *group, const bool enabled) {
    for (int i = 0; i < BLOCK_VERTEX_COUNT; i++) {
        group->vertices[i].enabled = enabled;
    }
}

static Vector2 realGetPieceQueueOffset(const CTetPieceType type) {
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

static CTetPoint constexpr GAME_FIELD_OFFSET = {-5, -10};

static void blockBatch_stageActive(const CTetEngine *engine, BlockBatch *batch) {
    const CTetPoint pieceOffset = ctEngine_getActivePiecePos(engine);
    const CTetPoint ghostPieceOffset = ctEngine_getGhostOffset(engine);
    const CTetPiece *piece = ctEngine_getActivePiece(engine);

    // Ghost CTetPiece
    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        CTetPoint coords = ctPoint_addToNew(pieceOffset, piece->coords[i]);
        ctPoint_add(&coords, ghostPieceOffset);

        setBlockColorWithCTetColor(&batch->field[coords.y][coords.x], piece->blocks[i].color);
        setBlockEnabled(&batch->field[coords.y][coords.x], true);
        setBlockBrightness(&batch->field[coords.y][coords.x], 0.4f);
    }
    // Active CTetPiece
    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        const CTetPoint coords = ctPoint_addToNew(pieceOffset, piece->coords[i]);

        setBlockColorWithCTetColor(&batch->field[coords.y][coords.x], piece->blocks[i].color);
        setBlockEnabled(&batch->field[coords.y][coords.x], true);
        constexpr float ratio = 0.7f;
        const float brightness = 1 - (1 - ctEngine_getLockDelayRemainingPercentage(engine)) * ratio;
        setBlockBrightness(&batch->field[coords.y][coords.x], brightness);
    }
}

static void blockBatch_stageNext(const CTetEngine *engine, BlockBatch *batch) {
    CTetPoint nextOffset = {6, 7};
    CTetPoint constexpr nextAdvance = {0, -3};
    const CTetPiece *nextPieces = ctEngine_getNextPieces(engine);

    for (int i = 0; i < CT_NEXT_QUEUE_MAX_LENGTH; i++) {
        const auto piece = nextPieces[i];
        const auto nextPieceOffset = realGetPieceQueueOffset(piece.type);
        for (int j = 0; j < PIECE_BLOCK_COUNT; j++) {
            
            auto coords = vector2_fromCtPoint(ctPoint_addToNew(nextOffset, piece.coords[j]));
            vector2_add(&coords, nextPieceOffset);
            
            setBlockVertices(&batch->nextPieces[i][j], coords);
            setBlockBrightness(&batch->nextPieces[i][j], 1);
            setBlockColorWithCTetColor(&batch->nextPieces[i][j], piece.blocks[j].color);
        }
        ctPoint_add(&nextOffset, nextAdvance);
    }
}

static void blockBatch_stageHold(const CTetEngine *engine, BlockBatch *batch) {
    constexpr CTetPoint holdOffset = {-9, 7};
    const CTetPiece heldPiece = *ctEngine_getHeldPiece(engine);

    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        const auto holdBlockGroup = &batch->holdPiece[i];
        
        if (heldPiece.type == CTetPieceType_NONE) {
            setBlockEnabled(holdBlockGroup, false);
            continue;
        }

        const auto coords = vector2_fromCtPoint(ctPoint_addToNew(holdOffset, heldPiece.coords[i]));
        auto offset = realGetPieceQueueOffset(heldPiece.type);
        vector2_add(&offset, {-1, 0});
        setBlockVertices(holdBlockGroup, vector2_addToNew(coords, offset));
        setBlockBrightness(holdBlockGroup, 1);
        setBlockEnabled(holdBlockGroup, true);
        if (ctEngine_holdIsLocked(engine)) {
            setBlockColor(holdBlockGroup, HOLD_LOCKED_COLOR);
        } else {
            setBlockColorWithCTetColor(holdBlockGroup, heldPiece.blocks[i].color);
        }
    }
}

static constexpr int LOCK_FLASH_TIMER = 35;

static void blockBatch_stageField(const CTetEngine *engine, BlockBatch *batch) {
    for (int y = 0; y < CT_TOTAL_FIELD_HEIGHT; y++) {
        for (int x = 0; x < CT_FIELD_WIDTH; x++) {
            const auto block = ctEngine_getBlockAtFieldLocation(engine, {x, y});
            if (block->color == CTetBlockColor_NONE) {
                setBlockEnabled(&batch->field[y][x], false);
                continue;
            }
            setBlockEnabled(&batch->field[y][x], true);
            if (ctEngine_getTimestamp(engine) - block->lockedTimestamp < LOCK_FLASH_TIMER) {
                setBlockBrightness(&batch->field[y][x], 1);
                setBlockColor(&batch->field[y][x], LOCK_FLASH_COLOR);
            } else {
                setBlockBrightness(&batch->field[y][x], 0.8f);
                setBlockColorWithCTetColor(&batch->field[y][x], block->color);    
            }
        }
    }
}

void blockBatch_stageAll(const CTetEngine *engine, BlockBatch *batch) {
    blockBatch_stageNext(engine, batch);
    blockBatch_stageHold(engine, batch);
    blockBatch_stageField(engine, batch);
    blockBatch_stageActive(engine, batch);
}

static void blockBatch_initFieldPositions(BlockBatch *batch) {
    for (int y = 0; y < CT_TOTAL_FIELD_HEIGHT; y++) {
        for (int x = 0; x < CT_FIELD_WIDTH; x++) {
            const auto coords = ctPoint_addToNew(GAME_FIELD_OFFSET, {x, y});
            setBlockVertices(&batch->field[y][x], vector2_fromCtPoint(coords));
        }
    }
}

static void blockBatch_initNextEnabled(BlockBatch *batch) {
    for (int i = 0; i < CT_NEXT_QUEUE_MAX_LENGTH; i++) {
        for (int j = 0; j < PIECE_BLOCK_COUNT; j++) {
            setBlockEnabled(&batch->nextPieces[i][j], true);
        }

    }
}

void blockBatch_init(BlockBatch *batch) {
    blockBatch_initFieldPositions(batch);
    blockBatch_initNextEnabled(batch);
}
