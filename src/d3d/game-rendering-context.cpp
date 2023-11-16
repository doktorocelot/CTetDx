#include "game-rendering-context.hpp"
#include "check-result.hpp"

D3D11_SUBRESOURCE_DATA
getD11SubresourceData(GameRenderingContext *ctx, ID3D11Device *device) {// Setup buffer desc
    D3D11_BUFFER_DESC bufferDesc{};

    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(BlockBatch);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // Setup initdata
    D3D11_SUBRESOURCE_DATA initData = {};

    initData.pSysMem = &ctx->blockBatch;

    HRESULT r;

    r = device->CreateBuffer(&bufferDesc, &initData, &ctx->blockMesh.vertexBuffer);
    checkResult(r, "CreateBuffer (Vertex)");
    return initData;
}

void gameRenderingContext_init(GameRenderingContext *ctx, ID3D11Device *device) {
    D3D11_BUFFER_DESC bufferDesc;
    HRESULT r;
    blockBatch_initFieldPositions(&ctx->blockBatch);
    blockBatch_initNextEnabled(&ctx->blockBatch);
    D3D11_SUBRESOURCE_DATA initData = getD11SubresourceData(ctx, device);

    constexpr int TOTAL_BLOCKS_IN_BATCH = sizeof(BlockBatch) / sizeof(BlockGroup);
    constexpr int BLOCK_BATCH_INDICES = TOTAL_BLOCKS_IN_BATCH * 6;
    bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(UINT) * BLOCK_BATCH_INDICES;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

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
            },
            {
                    "TEXCOORD",
                    0,
                    DXGI_FORMAT_R32_FLOAT,
                    0,
                    sizeof(float) * 3,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
            },
            {
                    "TEXCOORD",
                    1,
                    DXGI_FORMAT_R32_UINT,
                    0,
                    sizeof(float) * 3 + sizeof(float),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
            },
            {
                    "COLOR",
                    0,
                    DXGI_FORMAT_R32G32B32_FLOAT,
                    0,
                    sizeof(float) * 3 + sizeof(float) + sizeof(unsigned int),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
            }
    };
    shaderPair_init(&ctx->blockMesh.shaders, device, L"resources\\shaders\\BlockVertex.hlsl",
                    L"resources\\shaders\\BlockPixel.hlsl", layoutDesc, 4);
    ctx->blockMesh.stride = sizeof(BlockVertex);
    ctx->blockMesh.indices = BLOCK_BATCH_INDICES;

    // Frame Mesh

    const FrameVertex frameVertices[] = {
            {{-5,    -10,    0}},
            {{-5.25, -10,    0}},
            {{-5,    -10.25, 0}},
            {{-5.25, -10.25, 0}},

            {{5,     -10,    0}},
            {{5.25,  -10,    0}},
            {{5,     -10.25, 0}},
            {{5.25,  -10.25, 0}},

            {{-5,    10,     0}},
            {{-5.25, 10,     0}},
            {{-5,    10.25,  0}},
            {{-5.25, 10.25,  0}},

            {{5,     10,     0}},
            {{5.25,  10,     0}},
            {{5,     10.25,  0}},
            {{5.25,  10.25,  0}},
    };

    const UINT frameIndices[] = {
            3, 1, 0,
            3, 0, 2,
            2, 0, 4,
            2, 4, 6,
            6, 4, 5,
            6, 5, 7,
            4, 12, 13,
            4, 13, 5,
            12, 14, 15,
            12, 15, 13,
            8, 10, 14,
            8, 14, 12,
            9, 11, 10,
            9, 10, 8,
            1, 9, 8,
            1, 8, 0
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
    shaderPair_init(&ctx->frameMesh.shaders, device,
                    L"resources\\shaders\\FrameVertex.hlsl",
                    L"resources\\shaders\\FramePixel.hlsl",
                    frameLayoutDesc, 1);
    ctx->frameMesh.stride = sizeof(FrameVertex);
    ctx->frameMesh.indices = sizeof(frameIndices) / sizeof(UINT);
}

void updateBlockBatch(BlockBatch *batch, Mesh *mesh, CTetEngine *engine, ID3D11DeviceContext *deviceContext) {
    blockBatch_setupNext(engine, batch);
    blockBatch_setupHold(engine, batch);
    blockBatch_setupField(engine, batch);
    blockBatch_setupActive(engine, batch);

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

void gameRenderingContext_cleanup(GameRenderingContext *ctx) {
    mesh_cleanup(&ctx->blockMesh);
}

