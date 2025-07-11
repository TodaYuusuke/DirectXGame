#include "../../Structs.hlsli"

// ** for VS ** //

StructuredBuffer<Vertex> vVertex : register(t0);
StructuredBuffer<WorldTransform> vWorldTransform : register(t1);
ConstantBuffer<Camera> vCamera : register(b0);

// ** for PS ** //

StructuredBuffer<Material> pMaterial : register(t0);
Texture2D<float32_t4> pTexture[] : register(t0, space1);
SamplerState pSampler : register(s0);