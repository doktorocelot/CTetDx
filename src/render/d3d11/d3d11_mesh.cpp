#include "d3d11_mesh.hpp"

void mesh_use(D3d11Mesh *mesh, ID3D11DeviceContext *deviceContext) {
    shaderPair_use(&mesh->shaders, deviceContext);
    constexpr UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &mesh->stride, &offset);
    deviceContext->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    if (mesh->instanceBuffer != nullptr) {
        deviceContext->IASetVertexBuffers(1, 1, &mesh->instanceBuffer, &mesh->strideInstance, &offset);
    }
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void mesh_draw(const D3d11Mesh *mesh, ID3D11DeviceContext *deviceContext) {
    if (mesh->instanceBuffer != nullptr) {
        deviceContext->DrawIndexedInstanced(mesh->indices, mesh->instanceCount, 0, 0, 0);
    } else {
        deviceContext->DrawIndexed(mesh->indices, 0, 0);
    }
    
}

void mesh_cleanup(D3d11Mesh *mesh) {
    mesh->vertexBuffer->Release();
    mesh->indexBuffer->Release();
    mesh->instanceBuffer->Release();
    shaderPair_cleanup(&mesh->shaders);
}