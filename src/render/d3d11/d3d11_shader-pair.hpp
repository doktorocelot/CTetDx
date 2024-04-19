#pragma once

#include <d3d11.h>
#include <vector>

#define MAX_SHADER_PAIR_BUFFERS 3

struct D3d11ShaderPair {
    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *inputLayout;
    ID3D11Buffer *constantBuffersVs[MAX_SHADER_PAIR_BUFFERS];
    int constantBufferVsPtr;
    ID3D11Buffer *constantBuffersPs[MAX_SHADER_PAIR_BUFFERS];
    int constantBufferPsPtr;
};

void shaderPair_init(D3d11ShaderPair *pair, ID3D11Device *device, LPCWSTR vertexPath, LPCWSTR pixelPath,
                     D3D11_INPUT_ELEMENT_DESC *layoutDesc, int layoutElements);

void shaderPair_pushCBufferVs(D3d11ShaderPair *pair, ID3D11Buffer *buffer);
void shaderPair_pushCBufferPs(D3d11ShaderPair *pair, ID3D11Buffer *buffer);

void shaderPair_cleanup(D3d11ShaderPair *pair);

void shaderPair_use(D3d11ShaderPair *pair, ID3D11DeviceContext *deviceContext);