#include "MSStruct.hlsli"

// InstanceData
ConstantBuffer<InstanceData> InstanceData : register(b2);

// CommonBuffers
ConstantBuffer<CommonData> cCommonData : register(b0);
ConstantBuffer<Camera> cCamera : register(b1);
StructuredBuffer<Material> cMaterials : register(t4);
StructuredBuffer<DirectionalLight> cDLights : register(t5);
StructuredBuffer<PointLight> cPLights : register(t6);

Texture2D<float32_t4> tTexture[500] : register(t7);
Texture2D<float> tDLightSM[1] : register(t507);
TextureCube<float> tPLightSM[8] : register(t508);

SamplerState sTexSmp : register(s0);
SamplerState sDLightSMSmp : register(s1);
SamplerState sPLightSMSmp : register(s2);
