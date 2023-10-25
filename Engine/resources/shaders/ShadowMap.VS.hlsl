#include "Object3d.hlsli"

VertexShaderOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // インデックスを抽出
    uint32_t v = gIndex[vertexID + (instanceID * 3)].vertex;
    uint32_t w = gIndex[vertexID + (instanceID * 3)].worldMatrix;

    VertexShaderOutput output;
    output.position = mul(mul(gVertex[v].position, gWorldMatrix[w]), gDirectionalLight.viewProjection);
    return output;
}