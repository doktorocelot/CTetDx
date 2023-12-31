#include "d3d11_texture.hpp"

#include "../../render/bmp.hpp"
#include "../../win32/win32_check-result.hpp"
#include "../../win32/win32_files.hpp"
#include "../../win32/win32_kill-program.hpp"
#include "../../win32/win32_memory.hpp"

#include <string>

ID3D11Texture2D *d3d11_loadStaticTextureFromBmp(ID3D11Device *device, const wchar_t *path) {
    wchar_t filePath[MAX_PATH];

    win32_setCompleteFilePath(filePath, MAX_PATH, path);

    unsigned char *skinImgFileData = nullptr;
    BmpImage skinImage;

    if (win32_fileExists(filePath)) {
        HANDLE skinImgHandle = win32_openFile(filePath);
        skinImgFileData = win32_loadFileIntoNewVirtualBuffer(skinImgHandle);
        skinImage = bmp_init(
            skinImgFileData,
            static_cast<unsigned char *>(win32_allocateMemory(
                bmp_getBufferSize(skinImgFileData)
                ))
            );
        win32_deallocateMemory(skinImgFileData);
    } else {
        const std::wstring errorMessage = L"Could not load static texture " + std::wstring(path);
        win32_killProgram(errorMessage.c_str());
        return nullptr;
    }
    
    D3D11_TEXTURE2D_DESC textureDesc = {};

    textureDesc.Width = skinImage.width;
    textureDesc.Height = skinImage.height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA textureData;
    textureData.pSysMem = skinImage.imageData;
    textureData.SysMemPitch = skinImage.width * 4; // 4bpp

    ID3D11Texture2D *texture = nullptr;
    const HRESULT result = device->CreateTexture2D(&textureDesc, &textureData, &texture);
    win32_checkResult(result, "CreateTexture2D");

    win32_deallocateMemory(skinImage.imageData);
    
    return texture;
}

ID3D11ShaderResourceView *d3d11_createSrvFromTexture(ID3D11Device *device, ID3D11Texture2D *texture) {
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    ID3D11ShaderResourceView *srv = nullptr;
    const HRESULT result = device->CreateShaderResourceView(texture, &srvDesc, &srv);
    win32_checkResult(result, "CreateShaderResourceView");
    return srv;
}

ID3D11SamplerState *d3d11_createBlockSampler(ID3D11Device *device) {
    D3D11_SAMPLER_DESC samplerDesc = {};

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ID3D11SamplerState *samplerState = nullptr;
    const HRESULT result = device->CreateSamplerState(&samplerDesc, &samplerState);
    win32_checkResult(result, "CreateSamplerState");
    return samplerState;
}

void d3d11Texture_use(const D3d11Texture *texture, ID3D11DeviceContext *deviceContext) {
    deviceContext->PSSetShaderResources(0, 1, &texture->srv);
    deviceContext->PSSetSamplers(0, 1, &texture->sampler);
}

