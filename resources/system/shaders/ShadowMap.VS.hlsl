#include "Object3d.hlsli"

struct ShadowMapOutput {
    float4 position : SV_POSITION;
};

ShadowMapOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // インデックスを抽出
    uint32_t v = gIndex[vertexID + (instanceID * 3)].vertex;
    uint32_t w = gIndex[vertexID + (instanceID * 3)].worldTransform;

    ShadowMapOutput output;
    output.position = mul(mul(gVertex[v].position, gWorldTransform[w].wtf), gCameraData.m);
    return output;
}