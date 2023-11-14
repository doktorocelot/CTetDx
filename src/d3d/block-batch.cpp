#include "block-batch.hpp"

static DirectX::XMFLOAT3 colors[]{
        DirectX::XMFLOAT3(0, 0, 0),
        DirectX::XMFLOAT3(0.93, 0.16, 0.22),
        DirectX::XMFLOAT3(1, 0.47, 0),
        DirectX::XMFLOAT3(1, 0.79, 0),
        DirectX::XMFLOAT3(0.41, 0.75, 0.16),
        DirectX::XMFLOAT3(0, 0.62, 0.85),
        DirectX::XMFLOAT3(0, 0.39, 0.74),
        DirectX::XMFLOAT3(0.59, 0.18, 0.6),
};

void setBlockVertices(BlockGroup *group, float x, float y) {
    group->vertices[0].position = DirectX::XMFLOAT3(x, y, 0.0f);
    group->vertices[1].position = DirectX::XMFLOAT3(x + 1, y, 0.0f);
    group->vertices[2].position = DirectX::XMFLOAT3(x, y + 1, 0.0f);
    group->vertices[3].position = DirectX::XMFLOAT3(x + 1, y + 1, 0.0f);
}

void setBlockColor(BlockGroup *group, CTetBlockColor color) {
    auto colorVal = colors[color];
    group->vertices[0].color = colorVal;
    group->vertices[1].color = colorVal;
    group->vertices[2].color = colorVal;
    group->vertices[3].color = colorVal;
}

void setBlockEnabled(BlockGroup *group, bool enabled) {
    group->vertices[0].enabled = enabled;
    group->vertices[1].enabled = enabled;
    group->vertices[2].enabled = enabled;
    group->vertices[3].enabled = enabled;
}

void setBlockBrightness(BlockGroup *group, float brightness) {
    group->vertices[0].brightness = brightness;
    group->vertices[1].brightness = brightness;
    group->vertices[2].brightness = brightness;
    group->vertices[3].brightness = brightness;
}

static CTetPoint getPieceQueueOffset(const CTetPieceType type) {
    CTetPoint additionalOffset;
    switch (type) {
        case CTetPieceType_O:
            additionalOffset = {1, 1};
            break;
        default:
            additionalOffset = {0};
            break;
    }
    return additionalOffset;
}

static CTetPoint constexpr GAME_FIELD_OFFSET = {-5, -10};

void blockBatch_setupActive(CTetEngine *engine, BlockBatch *batch) {
    const CTetPoint pieceOffset = ctEngine_getActivePiecePos(engine);
    const CTetPoint ghostPieceOffset = ctEngine_getGhostOffset(engine);
    const CTetPiece *piece = ctEngine_getActivePiece(engine);

    // Ghost CTetPiece
    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        CTetPoint coords = ctPoint_addToNew(pieceOffset, piece->coords[i]);
        ctPoint_add(&coords, ghostPieceOffset);

        setBlockColor(&batch->field[coords.y][coords.x], piece->blocks[i].color);
        setBlockEnabled(&batch->field[coords.y][coords.x], true);
        setBlockBrightness(&batch->field[coords.y][coords.x], 0.4f);
    }
    // Active CTetPiece
    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        CTetPoint coords = ctPoint_addToNew(pieceOffset, piece->coords[i]);

        setBlockColor(&batch->field[coords.y][coords.x], piece->blocks[i].color);
        setBlockEnabled(&batch->field[coords.y][coords.x], true);
        constexpr float ratio = 0.7f;
        float brightness = 1 - (1 - ctEngine_getLockDelayRemainingPercentage(engine)) * ratio;
        setBlockBrightness(&batch->field[coords.y][coords.x], brightness);
    }
}

void blockBatch_setupNext(CTetEngine *engine, BlockBatch *batch) {
    CTetPoint nextOffset = {6, 7};
    CTetPoint constexpr nextAdvance = {0, -3};
    const CTetPiece *nextPieces = ctEngine_getNextPieces(engine);

    for (int i = 0; i < CT_NEXT_QUEUE_MAX_LENGTH; i++) {
        auto piece = nextPieces[i];
        auto nextPieceOffset = getPieceQueueOffset(piece.type);
        for (int j = 0; j < PIECE_BLOCK_COUNT; j++) {
            auto coords = ctPoint_addToNew(nextOffset, piece.coords[j]);
            ctPoint_add(&coords, nextPieceOffset);
            setBlockVertices(&batch->nextPieces[i][j], static_cast<float>(coords.x), static_cast<float>(coords.y));
            setBlockBrightness(&batch->nextPieces[i][j], 1);
            setBlockColor(&batch->nextPieces[i][j], piece.blocks[j].color);
        }
        ctPoint_add(&nextOffset, nextAdvance);
    }
}

void blockBatch_setupHold(CTetEngine *engine, BlockBatch *batch) {
    CTetPoint holdOffset = {-9, 7};
    CTetPiece heldPiece = *ctEngine_getHeldPiece(engine);

    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        if (heldPiece.type == CTetPieceType_NONE) {
            setBlockEnabled(&batch->holdPiece[i], false);
            continue;
        }
        auto coords = ctPoint_addToNew(holdOffset, heldPiece.coords[i]);
        ctPoint_add(&coords, getPieceQueueOffset(heldPiece.type));
        setBlockVertices(&batch->holdPiece[i], static_cast<float>(coords.x), static_cast<float>(coords.y));
        setBlockBrightness(&batch->holdPiece[i], 1);
        setBlockEnabled(&batch->holdPiece[i], true);
        setBlockColor(&batch->holdPiece[i], heldPiece.blocks[i].color);
    }
}

void blockBatch_setupField(CTetEngine *engine, BlockBatch *batch) {
    for (int y = 0; y < CT_TOTAL_FIELD_HEIGHT; y++) {
        for (int x = 0; x < CT_FIELD_WIDTH; x++) {
            CTetBlockColor color = ctEngine_getBlockAtFieldLocation(engine, {x, y})->color;
            if (color == CTetBlockColor_NONE) {
                setBlockEnabled(&batch->field[y][x], false);
                continue;
            }
            setBlockBrightness(&batch->field[y][x], 0.8f);
            setBlockEnabled(&batch->field[y][x], true);
            setBlockColor(&batch->field[y][x], color);
        }
    }
}

void blockBatch_initFieldPositions(BlockBatch *batch) {
    for (int y = 0; y < CT_TOTAL_FIELD_HEIGHT; y++) {
        for (int x = 0; x < CT_FIELD_WIDTH; x++) {
            auto coords = ctPoint_addToNew(GAME_FIELD_OFFSET, {x, y});
            setBlockVertices(&batch->field[y][x], static_cast<float>(coords.x), static_cast<float>(coords.y));
        }
    }
}

void blockBatch_initNextEnabled(BlockBatch *batch) {
    for (int i = 0; i < CT_NEXT_QUEUE_MAX_LENGTH; i++) {
        for (int j = 0; j < PIECE_BLOCK_COUNT; j++) {
            setBlockEnabled(&batch->nextPieces[i][j], true);
        }

    }
}

