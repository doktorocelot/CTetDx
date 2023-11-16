#include "mesh.hpp"

void mesh_use(Mesh *mesh, ID3D11DeviceContext *deviceContext) {
    shaderPair_use(&mesh->shaders, deviceContext);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &mesh->stride, &offset);
    deviceContext->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void mesh_draw(Mesh *mesh, ID3D11DeviceContext *deviceContext) {
    deviceContext->DrawIndexed(mesh->indices, 0, 0);
}

void mesh_cleanup(Mesh *mesh) {
    mesh->vertexBuffer->Release();
    mesh->indexBuffer->Release();
    shaderPair_cleanup(&mesh->shaders);
}