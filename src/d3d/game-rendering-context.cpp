#include "game-rendering-context.hpp"
#include "check-result.hpp"
#include "d3d-render.hpp"
#include "block-batch.hpp"
#include "frame-mesh.hpp"

void gameRenderingContext_init(GameRenderingContext *ctx, ID3D11Device *device) {
    createBlockBatchMesh(&ctx->blockBatch, &ctx->blockMesh, device);
    createFrameMesh(&ctx->frameMesh, device);
}

void updateBlockBatch(BlockBatch *batch, Mesh *mesh, CTetEngine *engine, ID3D11DeviceContext *deviceContext) {
    blockBatch_setupNext(engine, batch);
    blockBatch_setupHold(engine, batch);
    blockBatch_setupField(engine, batch);
    blockBatch_setupActive(engine, batch);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto r = deviceContext->Map(
            mesh->vertexBuffer,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mappedResource
    );
    checkResult(r, "MapResource (BlockBatch)");

    memcpy(mappedResource.pData, batch, sizeof(BlockBatch));

    deviceContext->Unmap(mesh->vertexBuffer, 0);
}

void gameRenderingContext_cleanup(GameRenderingContext *ctx) {
    mesh_cleanup(&ctx->blockMesh);
    mesh_cleanup(&ctx->frameMesh);
}

