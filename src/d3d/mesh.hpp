#pragma once

#include <d3d11.h>
#include "shader-pair.hpp"

struct Mesh {
    ID3D11Buffer *vertexBuffer;
    ID3D11Buffer *indexBuffer;
    ShaderPair shaders;
    UINT stride;
    UINT indices;
};

void mesh_use(Mesh *mesh, ID3D11DeviceContext *deviceContext);

void mesh_draw(Mesh *mesh, ID3D11DeviceContext *deviceContext);

void mesh_cleanup(Mesh *mesh);

