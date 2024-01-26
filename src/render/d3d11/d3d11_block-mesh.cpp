#include "d3d11_block-mesh.hpp"
#include "d3d11_renderer.hpp"

static void createBlockBatchIndexBuffer(D3d11Mesh *blockMesh, ID3D11Device *device) {
    const UINT indices[] = {0, 2, 1, 3, 1, 2};
    d3d11_createBuffer(device, indices, &blockMesh->indexBuffer, {
            .ByteWidth = sizeof(indices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
    });
    blockMesh->indices = 6;
}

static void createBlockBatchVertexBuffer(D3d11Mesh *blockMesh, ID3D11Device *device) {
    constexpr Vector2 vertices[] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    d3d11_createBuffer(device, vertices, &blockMesh->vertexBuffer, {
            .ByteWidth = sizeof(vertices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
    });
    blockMesh->stride = sizeof(Vector2);
}

static void createBlockBatchInstanceBuffer(const BlockInstance *instances, D3d11Mesh *blockMesh, ID3D11Device *device) {
    
    d3d11_createBuffer(device, instances, &blockMesh->instanceBuffer, {
            .ByteWidth = sizeof(BlockInstance) * TOTAL_BLOCK_INSTANCES,
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    });
    blockMesh->strideInstance = sizeof(BlockInstance);
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
                "POSITION_INSTANCE",
                0,
                DXGI_FORMAT_R32G32_FLOAT,
                1,
                0,
                D3D11_INPUT_PER_INSTANCE_DATA,
                1,
            },
            // {
            //         "BRIGHTNESS",
            //         0,
            //         DXGI_FORMAT_R32_FLOAT,
            //         0,
            //         sizeof(Vector2),
            //         D3D11_INPUT_PER_VERTEX_DATA,
            //         0,
            // },
            // {
            //         "ENABLED",
            //         0,
            //         DXGI_FORMAT_R32_UINT,
            //         0,
            //         sizeof(Vector2) + sizeof(float),
            //         D3D11_INPUT_PER_VERTEX_DATA,
            //         0,
            // },
            // {
            //         "TEXCOORD",
            //         0,
            //         DXGI_FORMAT_R32G32_FLOAT,
            //         0,
            //         sizeof(Vector2) + sizeof(float) + sizeof(unsigned int),
            //         D3D11_INPUT_PER_VERTEX_DATA,
            //         0,
            //     
            // },
            // {
            //         "ADDCOLOR",
            //         0,
            //         DXGI_FORMAT_R32G32B32_FLOAT,
            //         0,
            //         sizeof(Vector2) + sizeof(float) + sizeof(unsigned int) + sizeof(Vector2),
            //         D3D11_INPUT_PER_VERTEX_DATA,
            //         0,
            // }
    };
    shaderPair_init(&blockMesh->shaders, device, L"resources\\shaders\\BlockVertex.hlsl",
                    L"resources\\shaders\\BlockPixel.hlsl", layoutDesc, sizeof(layoutDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC));
    shaderPair_pushCBufferVs(&blockMesh->shaders, aspectRatioBuffer);
}

void createBlockMesh(const BlockBatch *blockBatch, D3d11Mesh *blockMesh, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer) {
    createBlockBatchVertexBuffer(blockMesh, device);
    createBlockBatchIndexBuffer(blockMesh, device);
    createBlockBatchInstanceBuffer(blockBatch->blockInstances, blockMesh, device);

    createBlockMeshShader(blockMesh, device, aspectRatioBuffer);
}