#include "../../Structs.hlsli"

// ** for VS ** //

StructuredBuffer<Vertex> vVertex : register(t0);
StructuredBuffer<WorldTransform> vWorldTransform : register(t1);
ConstantBuffer<Camera> vCamera : register(b0);
StructuredBuffer<int32_t> vSortIndex : register(t2);
StructuredBuffer<int32_t> vBillboardType : register(t3);
StructuredBuffer<float32_t3> vVelocity : register(t4);  // for StretchedBillboard

// ** for PS ** //

StructuredBuffer<Material> pMaterial : register(t0);
Texture2D<float32_t4> pTexture[] : register(t0, space1);
SamplerState pSampler : register(s0);


static const uint32_t kNumInstance = 6;
static const uint32_t kVertexID[kNumInstance] = {
    0, 1, 2, 0, 2, 3
};

Vertex GetVertexIndex(uint32_t vertexID, uint32_t instanceID) {
    return vVertex[(instanceID * 4) + kVertexID[vertexID]];
}
float32_t4 GetCenterPosition(uint32_t instanceID) {
    float32_t4 result = { 0.0f, 0.0f, 0.0f, 0.0f };
    result += vVertex[(instanceID * 4) + 0].position;
    result += vVertex[(instanceID * 4) + 1].position;
    result += vVertex[(instanceID * 4) + 2].position;
    result += vVertex[(instanceID * 4) + 3].position;
    
    result = result / 4.0f;
    result.w = 1.0f;
    return result;
}
float32_t3 GetWorldCenterPosition(uint32_t instanceID) {
    return mul(GetCenterPosition(instanceID), vWorldTransform[instanceID].m).xyz;
}