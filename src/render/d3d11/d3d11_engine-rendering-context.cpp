#include "d3d11_engine-rendering-context.hpp"
#include "../../win32/win32_check-result.hpp"
#include "d3d11_renderer.hpp"
#include "../block-batch.hpp"
#include "d3d11_block-mesh.hpp"
#include "d3d11_frame-mesh.hpp"
#include "d3d11_texture.hpp"

void d3d11EngineRenderingCtx_init(D3d11EngineRenderingCtx *ctx, ID3D11Device *device, ID3D11Buffer *aspectRatioBuffer) {
    blockBatch_init(&ctx->blockBatch);
    createBlockMesh(&ctx->blockBatch, &ctx->blockMesh, device, aspectRatioBuffer);
    createFrameMesh(&ctx->frameMesh, device, aspectRatioBuffer);

    auto blockSkinTex = d3d11_loadBlockSkin(device);
    ctx->blockSkinTexture.texture = blockSkinTex;
    ctx->blockSkinTexture.srv = d3d11_createSrvFromTexture(device, blockSkinTex);
    ctx->blockSkinTexture.sampler = d3d11_createBlockSampler(device);
}

void updateBlockBatchInMesh(BlockBatch *batch, D3d11Mesh *mesh, CTetEngine *engine, ID3D11DeviceContext *deviceContext) {
    blockBatch_stageAll(engine, batch);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto r = deviceContext->Map(
            mesh->vertexBuffer,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mappedResource
    );
    win32_checkResult(r, "MapResource (BlockBatch)");

    memcpy(mappedResource.pData, batch, sizeof(BlockBatch));

    deviceContext->Unmap(mesh->vertexBuffer, 0);
}

void d3d11EngineRenderingCtx_cleanup(D3d11EngineRenderingCtx *ctx) {
    mesh_cleanup(&ctx->blockMesh);
    mesh_cleanup(&ctx->frameMesh);
}

