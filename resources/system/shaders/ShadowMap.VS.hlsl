#include "Struct.hlsli"

struct ShadowMapOutput {
    float4 position : SV_POSITION;
};
struct ViewProjection {
    float32_t4x4 m;
};

StructuredBuffer<IndexInfo> gIndex : register(t0);
ConstantBuffer<ViewProjection> gViewProjection : register(b0);
StructuredBuffer<Vertex> gVertex : register(t1);
StructuredBuffer<WorldTransform> gWorldTransform : register(t2);


ShadowMapOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // インデックスを抽出
    uint32_t v = gIndex[vertexID + (instanceID * 3)].vertex;
    uint32_t w = gIndex[vertexID + (instanceID * 3)].worldTransform;

    ShadowMapOutput output;
    output.position = mul(mul(gVertex[v].position, gWorldTransform[w].wtf), gViewProjection.m);
    //output.position = mul(gVertex[v].position, gWorldTransform[w].wtf);
    return output;
}