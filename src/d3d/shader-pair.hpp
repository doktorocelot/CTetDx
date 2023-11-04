#pragma once

#include <d3d11.h>

struct ShaderPair {
    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *inputLayout;
};

void shaderPair_init(ShaderPair *pair, ID3D11Device *device, LPCWSTR vertexPath, LPCWSTR pixelPath,
                     D3D11_INPUT_ELEMENT_DESC *layoutDesc);

void shaderPair_cleanup(ShaderPair *pair);

void shaderPair_use(ShaderPair *pair, ID3D11DeviceContext *deviceContext);