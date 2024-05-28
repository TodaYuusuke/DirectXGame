#include "MSStruct.hlsli"


// CommonBuffers
ConstantBuffer<CommonData> cCommonData : register(b1);
ConstantBuffer<Camera> cCamera : register(b2);
StructuredBuffer<Material> cMaterials : register(t5);
StructuredBuffer<DirectionalLight> cDLights : register(t6);
StructuredBuffer<PointLight> cPLights : register(t7);

Texture2D<float32_t4> tTexture[500] : register(t8);
Texture2D<float> tDLightSM[1] : register(t508);
TextureCube<float> tPLightSM[8] : register(t509);

SamplerState sTexSmp : register(s0);
SamplerState sDLightSMSmp : register(s1);
SamplerState sPLightSMSmp : register(s2);
