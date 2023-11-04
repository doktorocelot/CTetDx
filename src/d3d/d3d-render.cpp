#include "d3d-render.hpp"
#include "check-result.hpp"

static void createRenderTargetView(
        IDXGISwapChain *swapChain,
        ID3D11Device *device,
        ID3D11DeviceContext *deviceContext,
        ID3D11RenderTargetView **target
) {
    ID3D11Texture2D *backBuffer = nullptr;
    HRESULT r;
    r = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &backBuffer);
    checkResult(r, "SwapChain GetBuffer");
    r = device->CreateRenderTargetView(backBuffer, nullptr, target);
    checkResult(r, "Device CreateRenderTargetView");
    backBuffer->Release();
    
    deviceContext->OMSetRenderTargets(1, target, nullptr);
}

void renderer_init(Renderer *renderer, HWND window, int width, int height) {
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView *target = nullptr;

    HRESULT r;
    
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = window;
    swapChainDesc.SampleDesc.Count = 4;
    swapChainDesc.Windowed = TRUE;
    r = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_DEBUG,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &swapChainDesc,
            &swapChain,
            &device,
            nullptr,
            &deviceContext
    );
    checkResult(r, "CreateDeviceAndSwapChain");

    createRenderTargetView(swapChain, device, deviceContext, &target);
    
    D3D11_VIEWPORT viewport = {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = (float) width,
            .Height = (float) height,
    };
    deviceContext->RSSetViewports(1, &viewport);
    
    renderer->swapChain = swapChain;
    renderer->renderTarget = target;
    renderer->deviceContext = deviceContext;
    renderer->device = device;
}

void renderer_cleanup(Renderer *renderer) {
    renderer->swapChain->Release();
    renderer->renderTarget->Release();
    renderer->device->Release();
    renderer->deviceContext->Release();
}

void renderer_drawFrame(Renderer *renderer, Engine *engine, GameRenderingContext *context) {
    float clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    renderer->deviceContext->ClearRenderTargetView(renderer->renderTarget, clearColor);

    mesh_use(&context->frameMesh, renderer->deviceContext);
    mesh_draw(&context->frameMesh, renderer->deviceContext);
    
    updateBlockBatch(&context->blockBatch, &context->blockMesh, engine, renderer->deviceContext);
    mesh_use(&context->blockMesh, renderer->deviceContext);
    mesh_draw(&context->blockMesh, renderer->deviceContext);
    
    renderer->swapChain->Present(0, 0);
}

