#include "../../Structs.hlsli"

// ** for VS ** //

StructuredBuffer<Vertex> vVertex : register(t0);
StructuredBuffer<WorldTransform> vWorldTransform : register(t1);
ConstantBuffer<Camera> vCamera : register(b0);
StructuredBuffer<int32_t> vSortIndex : register(t2);

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