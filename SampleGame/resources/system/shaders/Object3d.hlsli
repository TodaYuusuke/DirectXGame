#include "Struct.hlsli"

// ** for VS and PS ** //

StructuredBuffer<IndexInfo> gIndex : register(t0);
ConstantBuffer<CameraData> gCameraData : register(b0);

ConstantBuffer<CommonData> gCommonData : register(b1);

// ** for VS ** //

StructuredBuffer<Vertex> gVertex : register(t1);
StructuredBuffer<WorldTransform> gWorldTransform : register(t2);


// ** for PS ** //

StructuredBuffer<Material> gMaterial : register(t1);

StructuredBuffer<DirectionalLight> gDirectionalLight : register(t2);
StructuredBuffer<PointLight> gPointLight : register(t3);

Texture2D<float32_t4> gTexture[] : register(t0, space1);
SamplerState gSampler : register(s0);

Texture2D<float> gDirectionShadowMap[] : register(t0, space2);
SamplerState gDirectionShadowMapSampler : register(s1);
TextureCube<float> gPointShadowMap[] : register(t0, space3);
SamplerState gPointShadowMapSampler : register(s2);


const static float kShadowDensity = 0.2f;

