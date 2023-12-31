#pragma once

#include "d3d11_mesh.hpp"

struct FrameVertex {
    Vector3 position;
};

void createFrameMesh(D3d11Mesh *frameMesh, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer);