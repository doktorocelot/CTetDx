#include "d3d-game.hpp"
#include "check-result.hpp"

static constexpr int BLOCK_BATCH_BLOCKS = 4;
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
    shaderPair_init(&ctx->blockMesh.shaders, device, L"resources/shaders/TestVertex.hlsl", L"resources/shaders/TestPixel.hlsl", layoutDesc);
    ctx->blockMesh.stride = sizeof(BlockVertex);
    ctx->blockMesh.indices = BLOCK_BATCH_INDICES;
}

void setBlockVertices(BlockGroup *group, float x, float y) {
    group->vertices[0].position = DirectX::XMFLOAT3(x, y, 0.0f);
    group->vertices[1].position = DirectX::XMFLOAT3(x + 1, y, 0.0f);
    group->vertices[2].position = DirectX::XMFLOAT3(x, y + 1, 0.0f);
    group->vertices[3].position = DirectX::XMFLOAT3(x + 1, y + 1, 0.0f);
}

void updateBlockBatch(BlockBatch *batch, Mesh *mesh, Engine *engine, ID3D11DeviceContext *deviceContext) {
    auto pieceOffset = engine->active.pos;
    Point boardOffset = {-5, -10};

    for (int i = 0; i < 4; i++) {
        auto coords = point_addToNew(pieceOffset, engine->active.piece.coords[i]);
        point_add(&coords, boardOffset);
        setBlockVertices(&batch->activePiece[i], static_cast<float>(coords.x), static_cast<float>(coords.y));
    }

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
