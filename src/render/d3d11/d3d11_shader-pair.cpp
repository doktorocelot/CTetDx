#include <d3dcompiler.h>
#include "d3d11_shader-pair.hpp"
#include "../../win32/win32_check-result.hpp"
#include "..\..\win32\win32_files.hpp"
#include "../../win32/win32_kill-program.hpp"
#include <Shlwapi.h>
#include <string>
#include <fstream>

static HRESULT compileShader(
    LPCWSTR filePath,
    LPCSTR shaderModel,
    ID3DBlob **shaderBlobOut
) {
    WCHAR completeFilePath[MAX_PATH];
    win32_setCompleteFilePath(completeFilePath, MAX_PATH, filePath);
    HRESULT result = S_OK;

    if (!fileExists(completeFilePath)) {
        std::wstring errorMessage = L"Could not load shader file: " + std::wstring(completeFilePath);
        win32_killProgram(errorMessage.c_str());
    }

    ID3DBlob *errorBlob = nullptr;

    result = D3DCompileFromFile(
        completeFilePath,
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
            OutputDebugStringA((char *)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return result;
    }

    if (errorBlob) errorBlob->Release();
    return S_OK;
}

void shaderPair_init(D3d11ShaderPair *pair, ID3D11Device *device, LPCWSTR vertexPath, LPCWSTR pixelPath,
                     D3D11_INPUT_ELEMENT_DESC *layoutDesc, int layoutElements) {
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
    win32_checkResult(r, "CreateVertexShader");

    r = device->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        &pair->pixelShader
    );
    win32_checkResult(r, "CreatePixelShader");

    r = device->CreateInputLayout(
        layoutDesc,
        layoutElements,
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &pair->inputLayout
    );
    win32_checkResult(r, "CreateInputLayer");

    vertexShaderBlob->Release();
    pixelShaderBlob->Release();

    pair->constantBufferVsPtr = 0;
}

void shaderPair_pushCBufferVs(D3d11ShaderPair *pair, ID3D11Buffer *buffer) {
    pair->constantBuffersVs[pair->constantBufferVsPtr++] = buffer;
}

void shaderPair_cleanup(D3d11ShaderPair *pair) {
    pair->vertexShader->Release();
    pair->pixelShader->Release();
    pair->inputLayout->Release();
}

void shaderPair_use(D3d11ShaderPair *pair, ID3D11DeviceContext *deviceContext) {
    deviceContext->VSSetShader(pair->vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pair->pixelShader, nullptr, 0);
    deviceContext->IASetInputLayout(pair->inputLayout);
    deviceContext->VSSetConstantBuffers(0, pair->constantBufferVsPtr + 1, pair->constantBuffersVs);
}
