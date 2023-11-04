#include <d3dcompiler.h>
#include "shader-pair.hpp"
#include "check-result.hpp"

static HRESULT compileShader(
        LPCWSTR filePath,
        LPCSTR shaderModel,
        ID3DBlob **shaderBlobOut
) {
    HRESULT result = S_OK;
    ID3DBlob *errorBlob = nullptr;
    result = D3DCompileFromFile(
            filePath,
            nullptr,
            nullptr,
            "main",
            shaderModel,
            0,
            0,
            shaderBlobOut,
            &errorBlob
    );
    if (FAILED(result)) {
        if (errorBlob) {
            OutputDebugStringA((char *) errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return result;
    }

    if (errorBlob) errorBlob->Release();
    return S_OK;
}

void shaderPair_init(ShaderPair *pair, ID3D11Device *device, LPCWSTR vertexPath, LPCWSTR pixelPath,
                     D3D11_INPUT_ELEMENT_DESC *layoutDesc) {
    ID3DBlob *vertexShaderBlob = nullptr;
    ID3DBlob *pixelShaderBlob = nullptr;
    compileShader(vertexPath, "vs_5_0", &vertexShaderBlob);
    compileShader(pixelPath, "ps_5_0", &pixelShaderBlob);
    HRESULT r;
    r = device->CreateVertexShader(
            vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(),
            nullptr,
            &pair->vertexShader
    );
    checkResult(r, "CreateVertexShader");
    
    r = device->CreatePixelShader(
            pixelShaderBlob->GetBufferPointer(),
            pixelShaderBlob->GetBufferSize(),
            nullptr,
            &pair->pixelShader
    );
    checkResult(r, "CreatePixelShader");
    
    r = device->CreateInputLayout(
            layoutDesc,
            1,
            vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(),
            &pair->inputLayout
            );
    checkResult(r, "CreateInputLayer");
    
    vertexShaderBlob->Release();
    pixelShaderBlob->Release();
}

void shaderPair_cleanup(ShaderPair *pair) {
    pair->vertexShader->Release();
    pair->pixelShader->Release();
    pair->inputLayout->Release();
}

void shaderPair_use(ShaderPair *pair, ID3D11DeviceContext *deviceContext) {
    deviceContext->VSSetShader(pair->vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pair->pixelShader, nullptr, 0);
    deviceContext->IASetInputLayout(pair->inputLayout);
}