#pragma once

#include <d3d11.h>
#include "d3d11_shader-pair.hpp"

struct D3d11Mesh {
    ID3D11Buffer *vertexBuffer;
    ID3D11Buffer *indexBuffer;
    ID3D11Buffer *instanceBuffer;
    D3d11ShaderPair shaders;
    UINT stride;
    UINT strideInstance;
    UINT indices;
    UINT instanceCount;
};

void mesh_use(D3d11Mesh *mesh, ID3D11DeviceContext *deviceContext);

void mesh_draw(const D3d11Mesh *mesh, ID3D11DeviceContext *deviceContext);

void mesh_cleanup(D3d11Mesh *mesh);

