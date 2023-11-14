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

void setBlockColor(BlockGroup *group, BlockColor color) {
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

static Point getPieceQueueOffset(const PieceType type) {
    Point additionalOffset;
    switch (type) {
        case PieceType_O:
            additionalOffset = {1, 1};
            break;
        default:
            additionalOffset = {0};
            break;
    }
    return additionalOffset;
}

static Point constexpr GAME_FIELD_OFFSET = {-5, -10};

void blockBatch_setupActive(Engine *engine, BlockBatch *batch) {
    const Point pieceOffset = engine->active.pos;
    const Point ghostPieceOffset = {0, -activePiece_getDistanceToGround(&engine->active)};

    // Ghost Piece
    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        Point coords = point_addToNew(pieceOffset, engine->active.piece.coords[i]);
        point_add(&coords, ghostPieceOffset);

        setBlockColor(&batch->field[coords.y][coords.x], engine->active.piece.blocks[i].color);
        setBlockEnabled(&batch->field[coords.y][coords.x], true);
        setBlockBrightness(&batch->field[coords.y][coords.x], 0.4f);
    }
    // Active Piece
    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        Point coords = point_addToNew(pieceOffset, engine->active.piece.coords[i]);

        setBlockColor(&batch->field[coords.y][coords.x], engine->active.piece.blocks[i].color);
        setBlockEnabled(&batch->field[coords.y][coords.x], true);
        setBlockBrightness(&batch->field[coords.y][coords.x], 1);
    }
}

void blockBatch_setupNext(Engine *engine, BlockBatch *batch) {
    Point nextOffset = {6, 7};
    Point constexpr nextAdvance = {0, -3};

    for (int i = 0; i < NEXT_QUEUE_LENGTH; i++) {
        auto piece = engine->nextQueue.pieces[i];
        auto nextPieceOffset = getPieceQueueOffset(piece.type);
        for (int j = 0; j < PIECE_BLOCK_COUNT; j++) {
            auto coords = point_addToNew(nextOffset, piece.coords[j]);
            point_add(&coords, nextPieceOffset);
            setBlockVertices(&batch->nextPieces[i][j], static_cast<float>(coords.x), static_cast<float>(coords.y));
            setBlockBrightness(&batch->nextPieces[i][j], 1);
            setBlockColor(&batch->nextPieces[i][j], piece.blocks[j].color);
        }
        point_add(&nextOffset, nextAdvance);
    }
}

void blockBatch_setupHold(Engine *engine, BlockBatch *batch) {
    Point holdOffset = {-9, 7};
    Piece holdPiece = engine->holdQueue.held;

    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        if (holdPiece.type == PieceType_NONE) {
            setBlockEnabled(&batch->holdPiece[i], false);
            continue;
        }
        auto coords = point_addToNew(holdOffset, holdPiece.coords[i]);
        point_add(&coords, getPieceQueueOffset(holdPiece.type));
        setBlockVertices(&batch->holdPiece[i], static_cast<float>(coords.x), static_cast<float>(coords.y));
        setBlockBrightness(&batch->holdPiece[i], 1);
        setBlockEnabled(&batch->holdPiece[i], true);
        setBlockColor(&batch->holdPiece[i], holdPiece.blocks[i].color);
    }
}

void blockBatch_setupField(Engine *engine, BlockBatch *batch) {
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            BlockColor color = engine->field.matrix[y][x].color;
            if (color == BlockColor_NONE) {
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
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            auto coords = point_addToNew(GAME_FIELD_OFFSET, {x, y});
            setBlockVertices(&batch->field[y][x], static_cast<float>(coords.x), static_cast<float>(coords.y));
        }
    }
}

void blockBatch_initNextEnabled(BlockBatch *batch) {
    for (int i = 0; i < NEXT_QUEUE_LENGTH; i++) {
        for (int j = 0; j < PIECE_BLOCK_COUNT; j++) {
            setBlockEnabled(&batch->nextPieces[i][j], true);
        }

    }
}

