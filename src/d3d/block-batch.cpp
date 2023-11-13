#include "block-batch.hpp"
void setBlockVertices(BlockGroup *group, float x, float y) {
    group->vertices[0].position = DirectX::XMFLOAT3(x, y, 0.0f);
    group->vertices[1].position = DirectX::XMFLOAT3(x + 1, y, 0.0f);
    group->vertices[2].position = DirectX::XMFLOAT3(x, y + 1, 0.0f);
    group->vertices[3].position = DirectX::XMFLOAT3(x + 1, y + 1, 0.0f);
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

    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        Point coords = point_addToNew(pieceOffset, engine->active.piece.coords[i]);
        point_add(&coords, GAME_FIELD_OFFSET);
        setBlockVertices(&batch->activePiece[i], static_cast<float>(coords.x), static_cast<float>(coords.y));
        setBlockBrightness(&batch->activePiece[i], 1);

        point_add(&coords, ghostPieceOffset);
        setBlockVertices(&batch->ghostPiece[i], static_cast<float>(coords.x), static_cast<float>(coords.y));
        setBlockBrightness(&batch->ghostPiece[i], 0.4f);
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
        }
        point_add(&nextOffset, nextAdvance);
    }
}

void blockBatch_setupHold(Engine *engine, BlockBatch *batch) {
    Point holdOffset = {-9, 7};
    Piece holdPiece = engine->holdQueue.held;

    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        if (holdPiece.type == PieceType_NONE) {
            batch->holdPiece[i] = {};
            continue;
        }
        auto coords = point_addToNew(holdOffset, holdPiece.coords[i]);
        point_add(&coords, getPieceQueueOffset(holdPiece.type));
        setBlockVertices(&batch->holdPiece[i], static_cast<float>(coords.x), static_cast<float>(coords.y));
        setBlockBrightness(&batch->holdPiece[i], 1);
    }
}

void blockBatch_setupField(Engine *engine, BlockBatch *batch) {
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (engine->field.matrix[y][x].color == BlockColor_NONE) {
                batch->field[y][x] = {};
                continue;
            }
            auto coords = point_addToNew(GAME_FIELD_OFFSET, {x, y});
            setBlockVertices(&batch->field[y][x], static_cast<float>(coords.x), static_cast<float>(coords.y));
            setBlockBrightness(&batch->field[y][x], 0.8f);
        }
    }
}