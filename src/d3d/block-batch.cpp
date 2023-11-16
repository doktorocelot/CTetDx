#include "d3d-render.hpp"
#include "check-result.hpp"
#include "game-rendering-context.hpp"
#include "block-batch.hpp"
#include "math-util.hpp"

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

void setBlockVertices(BlockGroup *group, DirectX::XMFLOAT2 position) {
    group->vertices[0].position = DirectX::XMFLOAT2(position.x, position.y);
    group->vertices[1].position = DirectX::XMFLOAT2(position.x + 1, position.y);
    group->vertices[2].position = DirectX::XMFLOAT2(position.x, position.y + 1);
    group->vertices[3].position = DirectX::XMFLOAT2(position.x + 1, position.y + 1);
}

void setBlockColor(BlockGroup *group, CTetBlockColor color) {
    auto colorVal = colors[color];
    for (int i = 0; i < BLOCK_VERTEX_COUNT; i++) {
        group->vertices[i].color = colorVal;
    }
}

void setBlockEnabled(BlockGroup *group, bool enabled) {
    for (int i = 0; i < BLOCK_VERTEX_COUNT; i++) {
        group->vertices[i].enabled = enabled;
    }
}

void setBlockBrightness(BlockGroup *group, float brightness) {
    for (int i = 0; i < BLOCK_VERTEX_COUNT; i++) {
        group->vertices[i].brightness = brightness;
    }
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

static DirectX::XMFLOAT2 realGetPieceQueueOffset(const CTetPieceType type) {
    DirectX::XMFLOAT2 additionalOffset{};
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
        auto nextPieceOffset = realGetPieceQueueOffset(piece.type);
        for (int j = 0; j < PIECE_BLOCK_COUNT; j++) {
            auto coordsDx = ctPointToDx(ctPoint_addToNew(nextOffset, piece.coords[j]));
            setBlockVertices(&batch->nextPieces[i][j], {coordsDx.x + nextPieceOffset.x, coordsDx.y + nextPieceOffset.y});
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
        auto coordsDx = ctPointToDx(ctPoint_addToNew(holdOffset, heldPiece.coords[i]));
        auto offset = realGetPieceQueueOffset(heldPiece.type);
        setBlockVertices(&batch->holdPiece[i], {coordsDx.x + offset.x - 1.0f, coordsDx.y + offset.y});
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
            setBlockVertices(&batch->field[y][x], ctPointToDx(coords));
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

static void createBlockBatchIndexBuffer(BlockBatch *blockBatch, Mesh *blockMesh, ID3D11Device *device) {
    constexpr int TOTAL_BLOCKS_IN_BATCH = sizeof(BlockBatch) / sizeof(BlockGroup);
    constexpr int BLOCK_BATCH_INDICES = TOTAL_BLOCKS_IN_BATCH * 6;

    UINT indices[BLOCK_BATCH_INDICES];
    int index = 0;
    for (int i = 0; i < TOTAL_BLOCKS_IN_BATCH; i++) {
        indices[i * 6] = index;
        indices[i * 6 + 1] = index + 2;
        indices[i * 6 + 2] = index + 1;
        indices[i * 6 + 3] = index + 3;
        indices[i * 6 + 4] = index + 1;
        indices[i * 6 + 5] = index + 2;
        index += 4;
    }

    createBuffer(device, indices, &blockMesh->indexBuffer, {
            .ByteWidth = sizeof(UINT) * BLOCK_BATCH_INDICES,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
    });

    blockMesh->indices = BLOCK_BATCH_INDICES;
}

static void createBlockBatchVertexBuffer(BlockBatch *blockBatch, Mesh *blockMesh, ID3D11Device *device) {
    createBuffer(device, blockBatch, &blockMesh->vertexBuffer, {
            .ByteWidth = sizeof(BlockBatch),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    });
    blockMesh->stride = sizeof(BlockVertex);
}

static void createBlockMeshShader(Mesh *blockMesh, ID3D11Device *device) {
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            {
                    "POSITION",
                    0,
                    DXGI_FORMAT_R32G32_FLOAT,
                    0,
                    0,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
            },
            {
                    "BRIGHTNESS",
                    0,
                    DXGI_FORMAT_R32_FLOAT,
                    0,
                    sizeof(float) * 2,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
            },
            {
                    "ENABLED",
                    0,
                    DXGI_FORMAT_R32_UINT,
                    0,
                    sizeof(float) * 2 + sizeof(float),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
            },
            {
                    "COLOR",
                    0,
                    DXGI_FORMAT_R32G32B32_FLOAT,
                    0,
                    sizeof(float) * 2 + sizeof(float) + sizeof(unsigned int),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
            }
    };
    shaderPair_init(&blockMesh->shaders, device, L"resources\\shaders\\BlockVertex.hlsl",
                    L"resources\\shaders\\BlockPixel.hlsl", layoutDesc, 4);
}

void createBlockBatchMesh(BlockBatch *blockBatch, Mesh *blockMesh, ID3D11Device *device) {
    blockBatch_initFieldPositions(blockBatch);
    blockBatch_initNextEnabled(blockBatch);

    createBlockBatchVertexBuffer(blockBatch, blockMesh, device);
    createBlockBatchIndexBuffer(blockBatch, blockMesh, device);

    createBlockMeshShader(blockMesh, device);
}
