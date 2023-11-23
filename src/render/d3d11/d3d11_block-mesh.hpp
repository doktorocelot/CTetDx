#pragma once
#include "../block-batch.hpp"
#include "d3d11_mesh.hpp"

void createBlockMesh(const BlockBatch *blockBatch, D3d11Mesh *blockMesh, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer);
