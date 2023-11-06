#include "d3d-game.hpp"
#include "check-result.hpp"


static constexpr int BLOCK_BATCH_ACTIVE = PIECE_BLOCK_COUNT;
static constexpr int BLOCK_BATCH_NEXT = PIECE_BLOCK_COUNT * NEXT_QUEUE_LENGTH;
static constexpr int BLOCK_BATCH_FIELD = PIECE_BLOCK_COUNT * FIELD_WIDTH * FIELD_HEIGHT;
static constexpr int BLOCK_BATCH_HOLD = PIECE_BLOCK_COUNT;
static constexpr int BLOCK_BATCH_BLOCKS = BLOCK_BATCH_ACTIVE + BLOCK_BATCH_NEXT + BLOCK_BATCH_FIELD;
static constexpr int BLOCK_BATCH_INDICES = BLOCK_BATCH_BLOCKS * 6;

void gameRenderingContext_init(GameRenderingContext *ctx, ID3D11Device *device) {
    D3D11_BUFFER_DESC bufferDesc{};

    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(BlockBatch);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initData = {};

    initData.pSysMem = &ctx->blockBatch;

    HRESULT r;

    r = device->CreateBuffer(&bufferDesc, &initData, &ctx->blockMesh.vertexBuffer);
    checkResult(r, "CreateBuffer (Vertex)");

    bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(UINT) * BLOCK_BATCH_INDICES;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    UINT indices[BLOCK_BATCH_INDICES];
    int index = 0;
    for (int i = 0; i < BLOCK_BATCH_BLOCKS; i++) {
        indices[i * 6] = index;
        indices[i * 6 + 1] = index + 2;
        indices[i * 6 + 2] = index + 1;
        indices[i * 6 + 3] = index + 3;
        indices[i * 6 + 4] = index + 1;
        indices[i * 6 + 5] = index + 2;
        index += 4;
    }
    initData.pSysMem = indices;

    r = device->CreateBuffer(&bufferDesc, &initData, &ctx->blockMesh.indexBuffer);
    checkResult(r, "CreateBuffer (Index)");

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            {
                    "POSITION",
                    0,
                    DXGI_FORMAT_R32G32B32_FLOAT,
                    0,
                    0,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
            }
    };
    shaderPair_init(&ctx->blockMesh.shaders, device, L"resources/shaders/TestVertex.hlsl",
                    L"resources/shaders/TestPixel.hlsl", layoutDesc);
    ctx->blockMesh.stride = sizeof(BlockVertex);
    ctx->blockMesh.indices = BLOCK_BATCH_INDICES;

    // Frame Mesh

    const FrameVertex frameVertices[] = {
            {{-5, -10, 0}},
            {{-5.25, -10, 0}},
            {{-5, -10.25, 0}},
            {{-5.25, -10.25, 0}},

            {{5, -10, 0}},
            {{5.25, -10, 0}},
            {{5, -10.25, 0}},
            {{5.25, -10.25, 0}},

            {{-5, 10, 0}},
            {{-5.25, 10, 0}},
            {{-5, 10.25, 0}},
            {{-5.25, 10.25, 0}},

            {{5, 10, 0}},
            {{5.25, 10, 0}},
            {{5, 10.25, 0}},
            {{5.25, 10.25, 0}},
    };
    
    const UINT frameIndices[] = {
            3,1,0,
            3,0,2,
            2,0,4,
            2,4,6,
            6,4,5,
            6,5,7,
            4,12,13,
            4,13,5,
            12,14,15,
            12,15,13,
            8,10,14,
            8,14,12,
            9,11,10,
            9,10,8,
            1,9,8,
            1,8,0
    };

    bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(frameVertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    initData.pSysMem = frameVertices;

    r = device->CreateBuffer(&bufferDesc, &initData, &ctx->frameMesh.vertexBuffer);
    checkResult(r, "CreateBuffer (Vertex)");

    bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(frameIndices);
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    initData.pSysMem = frameIndices;
    r = device->CreateBuffer(&bufferDesc, &initData, &ctx->frameMesh.indexBuffer);
    checkResult(r, "CreateBuffer (Index)");

    D3D11_INPUT_ELEMENT_DESC frameLayoutDesc[] = {
            {
                    "POSITION",
                    0,
                    DXGI_FORMAT_R32G32B32_FLOAT,
                    0,
                    0,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
            }
    };
    shaderPair_init(&ctx->frameMesh.shaders, device, L"resources/shaders/TestVertex.hlsl",
                    L"resources/shaders/TestPixel.hlsl", frameLayoutDesc);
    ctx->frameMesh.stride = sizeof(FrameVertex);
    ctx->frameMesh.indices = sizeof(frameIndices) / sizeof(UINT);
}

void setBlockVertices(BlockGroup *group, float x, float y) {
    group->vertices[0].position = DirectX::XMFLOAT3(x, y, 0.0f);
    group->vertices[1].position = DirectX::XMFLOAT3(x + 1, y, 0.0f);
    group->vertices[2].position = DirectX::XMFLOAT3(x, y + 1, 0.0f);
    group->vertices[3].position = DirectX::XMFLOAT3(x + 1, y + 1, 0.0f);
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

static Point constexpr FIELD_OFFSET = {-5, -10};

void drawActivePiece(Engine *engine, BlockBatch *batch) {
    auto pieceOffset = engine->active.pos;

    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        auto coords = point_addToNew(pieceOffset, engine->active.piece.coords[i]);
        point_add(&coords, FIELD_OFFSET);
        setBlockVertices(&batch->activePiece[i], static_cast<float>(coords.x), static_cast<float>(coords.y));
    }
}

void drawNextQueue(Engine *engine, BlockBatch *batch) {
    Point nextOffset = {6, 7};
    Point constexpr nextAdvance = {0, -3};

    for (int i = 0; i < NEXT_QUEUE_LENGTH; i++) {
        auto piece = engine->nextQueue.pieces[i];
        auto nextPieceOffset = getPieceQueueOffset(piece.type);
        for (int j = 0; j < PIECE_BLOCK_COUNT; j++) {
            auto coords = point_addToNew(nextOffset, piece.coords[j]);
            point_add(&coords, nextPieceOffset);
            setBlockVertices(&batch->nextPieces[i][j], static_cast<float>(coords.x), static_cast<float>(coords.y));
        }
        point_add(&nextOffset, nextAdvance);
    }
}

void drawHoldQueue(Engine *engine, BlockBatch *batch) {
    Point holdOffset = {-9, 7};
    auto holdPiece = engine->holdQueue.held;

    for (int i = 0; i < PIECE_BLOCK_COUNT; i++) {
        if (holdPiece.type == PieceType_NONE) {
            batch->holdPiece[i] = {};
            continue;
        }
        auto coords = point_addToNew(holdOffset, holdPiece.coords[i]);
        point_add(&coords, getPieceQueueOffset(holdPiece.type));
        setBlockVertices(&batch->holdPiece[i], static_cast<float>(coords.x), static_cast<float>(coords.y));
    }
}

void drawField(Engine *engine, BlockBatch *batch) {
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (engine->field.matrix[y][x].color == BlockColor_NONE) {
                batch->field[y][x] = {};
                continue;
            }
            auto coords = point_addToNew(FIELD_OFFSET, {x, y});
            setBlockVertices(&batch->field[y][x], static_cast<float>(coords.x), static_cast<float>(coords.y));
        }
    }
}

void updateBlockBatch(BlockBatch *batch, Mesh *mesh, Engine *engine, ID3D11DeviceContext *deviceContext) {
    // Active Piece

    drawActivePiece(engine, batch);
    // Next Queue

    drawNextQueue(engine, batch);
    // Hold Queue

    drawHoldQueue(engine, batch);
    // Field

    drawField(engine, batch);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto r = deviceContext->Map(
            mesh->vertexBuffer,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mappedResource
    );
    checkResult(r, "MapResource (BlockBatch)");

    memcpy(mappedResource.pData, batch, sizeof(BlockBatch));

    deviceContext->Unmap(mesh->vertexBuffer, 0);
}

void mesh_use(Mesh *mesh, ID3D11DeviceContext *deviceContext) {
    shaderPair_use(&mesh->shaders, deviceContext);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &mesh->stride, &offset);
    deviceContext->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void mesh_draw(Mesh *mesh, ID3D11DeviceContext *deviceContext) {
    deviceContext->DrawIndexed(mesh->indices, 0, 0);
}

void gameRenderingContext_cleanup(GameRenderingContext *ctx) {
    mesh_cleanup(&ctx->blockMesh);
}

void mesh_cleanup(Mesh *mesh) {
    mesh->vertexBuffer->Release();
    mesh->indexBuffer->Release();
    shaderPair_cleanup(&mesh->shaders);
}
