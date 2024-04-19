#include "d3d11_engine-rendering-context.hpp"
#include "../../win32/win32_check-result.hpp"
#include "d3d11_renderer.hpp"
#include "../block-batch.hpp"
#include "d3d11_block-mesh.hpp"
#include "d3d11_frame-mesh.hpp"
#include "d3d11_texture.hpp"
#include "../../win32/win32_files.hpp"
#include "../../win32/win32_kill-program.hpp"
#include "../../win32/win32_memory.hpp"

static void createTextMesh(D3d11Mesh *mesh, TextRenderer *textRenderer, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer) {
    // Create vertex
    d3d11_createBuffer(device, textRenderer->chars, &mesh->vertexBuffer, {
            .ByteWidth = sizeof(CharVertex) * CHAR_QUAD_VERT_COUNT * MAX_CHAR_QUADS,
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    });
    mesh->stride = sizeof(CharVertex);

    int indices[6 * MAX_CHAR_QUADS];
    int index = 0;
    for (int i = 0; i < MAX_CHAR_QUADS; i++) {
        indices[i * 6] = index;
        indices[i * 6 + 1] = index + 2;
        indices[i * 6 + 2] = index + 1;
        indices[i * 6 + 3] = index + 3;
        indices[i * 6 + 4] = index + 1;
        indices[i * 6 + 5] = index + 2;
        index += 4;
    }
    
    // Create index
    d3d11_createBuffer(device, indices, &mesh->indexBuffer, {
            .ByteWidth = sizeof(indices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
    });
    mesh->indices = 0;

    // Shader
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0,
        },
        {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            sizeof(float) * 2,
            D3D11_INPUT_PER_VERTEX_DATA,
            0,

        },
    };
    shaderPair_init(&mesh->shaders, device,
                    L"resources\\shaders\\IngameTextVertex.hlsl",
                    L"resources\\shaders\\IngameTextPixel.hlsl",
                    layoutDesc, sizeof(layoutDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC));

    shaderPair_pushCBufferVs(&mesh->shaders, aspectRatioBuffer);
}

void d3d11EngineRenderingCtx_init(D3d11EngineRenderingCtx *ctx, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer) {
    blockBatch_init(&ctx->blockBatch);
    
    createBlockMesh(&ctx->blockBatch, &ctx->blockMesh, device, aspectRatioBuffer);
    createFrameMesh(&ctx->frameMesh, device, aspectRatioBuffer);

    // block skin
    const auto blockSkinTex = d3d11_loadStaticTextureFromBmp(device, L"resources\\img\\skin.bmp");
    ctx->blockSkinTexture.texture = blockSkinTex;
    ctx->blockSkinTexture.srv = d3d11_createSrvFromTexture(device, blockSkinTex);
    ctx->blockSkinTexture.sampler = d3d11_createBasicSampler(device);

    // font texture
    const auto fontTex = d3d11_loadStaticTextureFromBmp(device, L"resources\\font\\font.bmp");
    ctx->fontTexture.texture = fontTex;
    ctx->fontTexture.srv = d3d11_createSrvFromTexture(device, fontTex);
    ctx->fontTexture.sampler = d3d11_createBasicSampler(device);

    // font data
    wchar_t fontFilePath[MAX_PATH];

    win32_setCompleteFilePath(fontFilePath, MAX_PATH, L"resources\\font\\font.ctf");

    unsigned char *fontData = nullptr;
    if (win32_fileExists(fontFilePath)) {
        const HANDLE fontDataHandle = win32_openFile(fontFilePath);
        fontData = win32_loadFileIntoNewVirtualBuffer(fontDataHandle);
        win32_closeFile(fontDataHandle);
    } else {
        win32_killProgram(L"Could not load font data from resources\\font\\font.bin");
        return;
    }    
    textRenderer_init(&ctx->textRenderer, fontData);
    win32_deallocateMemory(fontData);
    
    // font mesh
    createTextMesh(&ctx->textMesh, &ctx->textRenderer, device, aspectRatioBuffer);

    // font blendstate
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    const HRESULT result = device->CreateBlendState(&blendDesc, &ctx->textBlendState);
    win32_checkResult(result, "CreateBlendState");

    //text
    ingameText_init(&ctx->ingameText);
}

void updateTextMesh(const TextRenderer *textRenderer, D3d11Mesh *mesh, ID3D11DeviceContext *deviceContext) {
    mesh->indices = textRenderer->activeCharCount * 6;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto r = deviceContext->Map(
        mesh->vertexBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedResource
    );
    win32_checkResult(r, "MapResource (Text Ingame)");

    memcpy(mappedResource.pData, textRenderer->chars, sizeof(CharVertex) * textRenderer->activeCharCount * 6);

    deviceContext->Unmap(mesh->vertexBuffer, 0);
}

void updateBlockBatchInMesh(BlockBatch *batch, D3d11Mesh *mesh, CTetEngine *engine, ID3D11DeviceContext *deviceContext) {
    blockBatch_stageAll(engine, batch);
    mesh->instanceCount = batch->len;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto r = deviceContext->Map(
            mesh->instanceBuffer,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mappedResource
    );
    win32_checkResult(r, "MapResource (BlockBatch)");

    memcpy(mappedResource.pData, batch->blockInstances, sizeof(BlockInstance) * mesh->instanceCount);

    deviceContext->Unmap(mesh->instanceBuffer, 0);
}

void d3d11EngineRenderingCtx_cleanup(D3d11EngineRenderingCtx *ctx) {
    mesh_cleanup(&ctx->blockMesh);
    mesh_cleanup(&ctx->frameMesh);
}

