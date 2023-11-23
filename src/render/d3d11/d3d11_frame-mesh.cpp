#include "d3d11_engine-rendering-context.hpp"
#include "../../win32/win32_check-result.hpp"
#include "d3d11_renderer.hpp"
#include "../block-batch.hpp"
#include "d3d11_frame-mesh.hpp"

static const FrameVertex frameVertices[] = {
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

static const UINT frameIndices[] = {
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

void createFrameMesh(D3d11Mesh *frameMesh, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer) {
    // Create vertex
    d3d11_createBuffer(device, frameVertices, &frameMesh->vertexBuffer, {
            .ByteWidth = sizeof(frameVertices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
    });
    frameMesh->stride = sizeof(FrameVertex);

    // Create index
    d3d11_createBuffer(device, frameIndices, &frameMesh->indexBuffer, {
            .ByteWidth = sizeof(frameIndices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
    });
    frameMesh->indices = sizeof(frameIndices) / sizeof(UINT);

    // Shader
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
    shaderPair_init(&frameMesh->shaders, device,
                    L"resources\\shaders\\FrameVertex.hlsl",
                    L"resources\\shaders\\FramePixel.hlsl",
                    frameLayoutDesc, 1);

    shaderPair_pushCBufferVs(&frameMesh->shaders, aspectRatioBuffer);
}