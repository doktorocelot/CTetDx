#pragma once
#include <d3d11.h>

struct D3d11Texture {
    ID3D11Texture2D *texture;
    ID3D11ShaderResourceView *srv;
    ID3D11SamplerState *sampler;
};

ID3D11Texture2D *d3d11_loadStaticTextureFromBmp(ID3D11Device *device, const wchar_t *path, int *outHeight);
ID3D11ShaderResourceView *d3d11_createSrvFromTexture(ID3D11Device *device, ID3D11Texture2D *texture);
ID3D11SamplerState *d3d11_createBasicSampler(ID3D11Device *device);
void d3d11Texture_use(const D3d11Texture *texture, ID3D11DeviceContext *deviceContext);