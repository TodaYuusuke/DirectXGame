#include "MSStruct.hlsli"

//ConstantBuffer<ConstantData>     ConstantData : register(b0);
//ConstantBuffer<WorldTransform>   Transform    : register(b2);
//ConstantBuffer<Material>         material     : register(b3);

ConstantBuffer<CameraData> gCameraData : register(b0);
ConstantBuffer<ModelData> gModelData : register(b1);
ConstantBuffer<CommonData> gCommonData : register(b2);

// Required
StructuredBuffer<Meshlet>    Meshlets            : register(t0);
StructuredBuffer<Vertex> Vertices            : register(t1);
ByteAddressBuffer            UniqueVertexIndices : register(t2);
StructuredBuffer<uint32_t>   PrimitiveIndices    : register(t3);

// more
StructuredBuffer<Material> Materials : register(t4);
StructuredBuffer<DirectionalLight> DLight : register(t5);
StructuredBuffer<PointLight> PLight : register(t6);

Texture2D<float32_t4> gTexture[500] : register(t7);
SamplerState gSampler : register(s0);
Texture2D<float> gDirectionShadowMap[1] : register(t507);
SamplerState gDirectionShadowMapSampler : register(s1);
TextureCube<float> gPointShadowMap[8] : register(t508);
SamplerState gPointShadowMapSampler : register(s2);