#pragma once

#include <d3d11.h>
#include "d3d11_shader-pair.hpp"

struct D3d11Mesh {
    ID3D11Buffer *vertexBuffer;
    ID3D11Buffer *indexBuffer;
    D3d11ShaderPair shaders;
    UINT stride;
    UINT indices;
};

void mesh_use(D3d11Mesh *mesh, ID3D11DeviceContext *deviceContext);

void mesh_draw(D3d11Mesh *mesh, ID3D11DeviceContext *deviceContext);

void mesh_cleanup(D3d11Mesh *mesh);

