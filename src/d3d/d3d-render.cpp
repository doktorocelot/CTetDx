#include "game-rendering-context.hpp"
#include "d3d-render.hpp"
#include "check-result.hpp"
#include "mesh.hpp"


void createRenderTargetView(IDXGISwapChain *swapChain, ID3D11Device *device, ID3D11DeviceContext *deviceContext,
                            ID3D11RenderTargetView **target) {
    ID3D11Texture2D *backBuffer = nullptr;
    HRESULT r;
    r = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
    checkResult(r, "SwapChain GetBuffer");
    r = device->CreateRenderTargetView(backBuffer, nullptr, target);
    checkResult(r, "Device CreateRenderTargetView");
    backBuffer->Release();

    deviceContext->OMSetRenderTargets(1, target, nullptr);
}


void setViewport(int width, int height, ID3D11DeviceContext *deviceContext) {
    D3D11_VIEWPORT viewport = {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = (float) width,
            .Height = (float) height,
    };
    deviceContext->RSSetViewports(1, &viewport);
}

static void createAspectRatioBuffer(Renderer *renderer) {
    createBuffer(renderer->device, &renderer->aspectRatioBufferData, &renderer->aspectRatioBuffer, {
            .ByteWidth = sizeof(AspectConstantBuffer),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
            .MiscFlags = 0,
    });
}

void renderer_init(Renderer *renderer, HWND window, int width, int height) {
    ID3D11Device *device = nullptr;
    ID3D11DeviceContext *deviceContext = nullptr;
    IDXGISwapChain *swapChain = nullptr;
    ID3D11RenderTargetView *target = nullptr;

    HRESULT r;

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

    r = D3D11CreateDeviceAndSwapChain(
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
    checkResult(r, "CreateDeviceAndSwapChain");

    createRenderTargetView(swapChain, device, deviceContext, &target);

    setViewport(width, height, deviceContext);

    renderer->aspectRatioBufferData = {static_cast<float>(width) / static_cast<float>(height)};

    renderer->swapChain = swapChain;
    renderer->renderTarget = target;
    renderer->deviceContext = deviceContext;
    renderer->device = device;

    createAspectRatioBuffer(renderer);
}

void renderer_cleanup(Renderer *renderer) {
    renderer->aspectRatioBuffer->Release();
    renderer->deviceContext->ClearState();
    renderer->deviceContext->Flush();
    renderer->deviceContext->Release();
    renderer->swapChain->Release();
    renderer->renderTarget->Release();
    renderer->device->Release();
}

void renderer_drawFrame(Renderer *renderer, CTetEngine *engine, GameRenderingContext *context) {
    float clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    renderer->deviceContext->ClearRenderTargetView(renderer->renderTarget, clearColor);

    mesh_use(&context->frameMesh, renderer->deviceContext);
    mesh_draw(&context->frameMesh, renderer->deviceContext);

    updateBlockBatch(&context->blockBatch, &context->blockMesh, engine, renderer->deviceContext);
    mesh_use(&context->blockMesh, renderer->deviceContext);
    mesh_draw(&context->blockMesh, renderer->deviceContext);

    const HRESULT r = renderer->swapChain->Present(0, 0);
    checkResult(r, "SwapChain Present");
}

void renderer_setAspectRatio(Renderer *renderer) {
    D3D11_MAPPED_SUBRESOURCE mappedAspect;
    ID3D11Resource *aspectRatioBuffer = renderer->aspectRatioBuffer;
    renderer->deviceContext->Map(aspectRatioBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedAspect);

    memcpy(mappedAspect.pData, &renderer->aspectRatioBufferData, sizeof(float));

    renderer->deviceContext->Unmap(aspectRatioBuffer, 0);
}


void createBuffer(ID3D11Device *device, const void *initData, ID3D11Buffer **destBuffer,
                  D3D11_BUFFER_DESC bufferDesc) {// Setup buffer desc
    D3D11_SUBRESOURCE_DATA initDataDescriptor = {};

    initDataDescriptor.pSysMem = initData;

    const HRESULT r = device->CreateBuffer(&bufferDesc, &initDataDescriptor, destBuffer);
    checkResult(r, "CreateBuffer");
}
