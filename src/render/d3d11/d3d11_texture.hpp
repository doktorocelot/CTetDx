#pragma once
#include <d3d11.h>

ID3D11Texture2D *d3d11_loadBlockSkin(ID3D11Device *device);
ID3D11ShaderResourceView *d3d11_createSrvFromTexture(ID3D11Device *device, ID3D11Texture2D *texture);
ID3D11SamplerState *d3d11_createBlockSampler(ID3D11Device *device);