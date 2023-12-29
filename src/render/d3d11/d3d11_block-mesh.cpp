#include "d3d11_block-mesh.hpp"
#include "d3d11_renderer.hpp"

static void createBlockBatchIndexBuffer(D3d11Mesh *blockMesh, ID3D11Device *device) {
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

    d3d11_createBuffer(device, indices, &blockMesh->indexBuffer, {
            .ByteWidth = sizeof(UINT) * BLOCK_BATCH_INDICES,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
    });

    blockMesh->indices = BLOCK_BATCH_INDICES;
}

static void createBlockBatchVertexBuffer(const BlockBatch *blockBatch, D3d11Mesh *blockMesh, ID3D11Device *device) {
    d3d11_createBuffer(device, blockBatch, &blockMesh->vertexBuffer, {
            .ByteWidth = sizeof(BlockBatch),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    });
    blockMesh->stride = sizeof(BlockVertex);
}

static void createBlockMeshShader(D3d11Mesh *blockMesh, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer) {
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
                    "TEXCOORD",
                    0,
                    DXGI_FORMAT_R32G32_FLOAT,
                    0,
                    sizeof(float) * 2 + sizeof(float) + sizeof(unsigned int),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0,
                
            }
            // {
            //         "COLOR",
            //         0,
            //         DXGI_FORMAT_R32G32B32_FLOAT,
            //         0,
            //         sizeof(float) * 2 + sizeof(float) + sizeof(unsigned int),
            //         D3D11_INPUT_PER_VERTEX_DATA,
            //         0,
            // }
    };
    shaderPair_init(&blockMesh->shaders, device, L"resources\\shaders\\BlockVertex.hlsl",
                    L"resources\\shaders\\BlockPixel.hlsl", layoutDesc, 4);
    shaderPair_pushCBufferVs(&blockMesh->shaders, aspectRatioBuffer);
}

void createBlockMesh(const BlockBatch *blockBatch, D3d11Mesh *blockMesh, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer) {
    createBlockBatchVertexBuffer(blockBatch, blockMesh, device);
    createBlockBatchIndexBuffer(blockMesh, device);

    createBlockMeshShader(blockMesh, device, aspectRatioBuffer);
}