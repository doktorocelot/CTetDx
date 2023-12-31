#include "d3d11_engine-rendering-context.hpp"
#include "d3d11_renderer.hpp"
#include "../../win32/win32_check-result.hpp"
#include "d3d11_mesh.hpp"


void d3d11_createRenderTargetView(IDXGISwapChain *swapChain, ID3D11Device *device, ID3D11DeviceContext *deviceContext,
                                  ID3D11RenderTargetView **target) {
    ID3D11Texture2D *backBuffer = nullptr;
    HRESULT r;
    r = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
    win32_checkResult(r, "SwapChain GetBuffer");
    r = device->CreateRenderTargetView(backBuffer, nullptr, target);
    win32_checkResult(r, "Device CreateRenderTargetView");
    backBuffer->Release();

    deviceContext->OMSetRenderTargets(1, target, nullptr);
}


void d3d11_setViewport(int width, int height, ID3D11DeviceContext *deviceContext) {
    D3D11_VIEWPORT viewport = {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = static_cast<float>(width),
            .Height = static_cast<float>(height),
    };
    deviceContext->RSSetViewports(1, &viewport);
}

static void createAspectRatioBuffer(D3d11Renderer *renderer) {
    d3d11_createBuffer(renderer->device, &renderer->aspectRatioBufferData, &renderer->aspectRatioBuffer, {
            .ByteWidth = sizeof(AspectConstantBuffer),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
            .MiscFlags = 0,
    });
}

void d3d11Renderer_init(D3d11Renderer *renderer, HWND window, int width, int height) {
    ID3D11Device *device = nullptr;
    ID3D11DeviceContext *deviceContext = nullptr;
    IDXGISwapChain *swapChain = nullptr;
    ID3D11RenderTargetView *target = nullptr;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = window;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    UINT createDeviceFlags = 0;
    
#ifdef DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    auto r = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &swapChainDesc,
            &swapChain,
            &device,
            nullptr,
            &deviceContext
    );
    win32_checkResult(r, "CreateDeviceAndSwapChain");

    d3d11_createRenderTargetView(swapChain, device, deviceContext, &target);

    d3d11_setViewport(width, height, deviceContext);

    renderer->aspectRatioBufferData = {static_cast<float>(width) / static_cast<float>(height)};

    renderer->swapChain = swapChain;
    renderer->renderTarget = target;
    renderer->deviceContext = deviceContext;
    renderer->device = device;

    createAspectRatioBuffer(renderer);
}

void d3d11Renderer_drawFrame(D3d11Renderer *renderer, CTetEngine *engine, D3d11EngineRenderingCtx *context) {
    float clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    renderer->deviceContext->ClearRenderTargetView(renderer->renderTarget, clearColor);

    mesh_use(&context->frameMesh, renderer->deviceContext);
    mesh_draw(&context->frameMesh, renderer->deviceContext);

    updateBlockBatchInMesh(&context->blockBatch, &context->blockMesh, engine, renderer->deviceContext);

    d3d11Texture_use(&context->blockSkinTexture, renderer->deviceContext);
    mesh_use(&context->blockMesh, renderer->deviceContext);
    mesh_draw(&context->blockMesh, renderer->deviceContext);

    const HRESULT r = renderer->swapChain->Present(0, 0);
    win32_checkResult(r, "SwapChain Present");
}

void d3d11Renderer_setAspectRatio(D3d11Renderer *renderer) {
    D3D11_MAPPED_SUBRESOURCE mappedAspect;
    ID3D11Resource *aspectRatioBuffer = renderer->aspectRatioBuffer;
    renderer->deviceContext->Map(aspectRatioBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedAspect);

    memcpy(mappedAspect.pData, &renderer->aspectRatioBufferData, sizeof(float));

    renderer->deviceContext->Unmap(aspectRatioBuffer, 0);
}

void d3d11Renderer_resize(D3d11Renderer *renderer, int width, int height, bool isMinimized) {
    if (renderer->device != nullptr && isMinimized) {
        if (renderer->renderTarget != nullptr) {
            renderer->renderTarget->Release();
            renderer->renderTarget = nullptr;
        }

        if (renderer->swapChain != nullptr) {
            renderer->deviceContext->ClearState();
            renderer->deviceContext->Flush();
            const HRESULT r = renderer->swapChain->ResizeBuffers(
                2,
                width,
                height,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                0
            );
            win32_checkResult(r, "SwapChain Resize Buffers");

            renderer->aspectRatioBufferData = {static_cast<float>(width) / static_cast<float>(height)};
            d3d11Renderer_setAspectRatio(renderer);

            d3d11_createRenderTargetView(renderer->swapChain, renderer->device, renderer->deviceContext,
                                         &renderer->renderTarget);
            d3d11_setViewport(width, height, renderer->deviceContext);
        }
    }
}

void d3d11_createBuffer(ID3D11Device *device, const void *initData, ID3D11Buffer **destBuffer,
                  D3D11_BUFFER_DESC bufferDesc) {// Setup buffer desc
    D3D11_SUBRESOURCE_DATA initDataDescriptor = {};

    initDataDescriptor.pSysMem = initData;

    const HRESULT r = device->CreateBuffer(&bufferDesc, &initDataDescriptor, destBuffer);
    win32_checkResult(r, "CreateBuffer");
}
