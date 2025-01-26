#include "../../../system/shaders/utility/Particle.hlsli"
#include "../../../system/shaders/utility/Random.hlsli"
#include "../../../system/shaders/Struct.hlsli"

ConstantBuffer<PerFrame> cPerFrame : register(b0);
ConstantBuffer<EmitterSphere> cEmitter : register(b1);
RWStructuredBuffer<ParticleData> rParticleData : register(u0);