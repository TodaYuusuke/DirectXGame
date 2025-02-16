#include "../../system/shaders/utility/Particle.hlsli"
#include "../../system/shaders/utility/Random.hlsli"
#include "../../system/shaders/Struct.hlsli"

ConstantBuffer<PerFrame> cPerFrame : register(b0);
ConstantBuffer<EmitterSphere> cEmitter : register(b1);
ConstantBuffer<ParticleSize> cParticleSize : register(b2);
RWStructuredBuffer<ParticleData> rParticleData : register(u0);
RWStructuredBuffer<int32_t> rFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> rFreeList : register(u2);
StructuredBuffer<uint32_t> sHitList : register(t0);
