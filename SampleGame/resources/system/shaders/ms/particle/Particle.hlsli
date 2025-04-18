#include "../MSStruct.hlsli"
#include "../../utility/Particle.hlsli"

// CommonBuffers
ConstantBuffer<CommonData> cCommonData : register(b1);
ConstantBuffer<Camera> cCamera : register(b2);
ConstantBuffer<ParticleSize> cParticleSize : register(b3);
StructuredBuffer<ParticleData> sParticle : register(t5);
StructuredBuffer<Material> cMaterials : register(t6);
StructuredBuffer<DirectionalLight> cDLights : register(t7);
StructuredBuffer<PointLight> cPLights : register(t8);

Texture2D<float32_t4> tTexture[500] : register(t9);
Texture2D<float> tDLightSM[1] : register(t509);
TextureCube<float> tPLightSM[8] : register(t510);

SamplerState sTexSmp : register(s0);
SamplerState sDLightSMSmp : register(s1);
SamplerState sPLightSMSmp : register(s2);

