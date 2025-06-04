#include "../../utility/Particle.hlsli"

ConstantBuffer<ParticleSize> cParticleSize : register(b0);
RWStructuredBuffer<ParticleData> rParticleData : register(u0);
RWStructuredBuffer<int32_t> rFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> rFreeList : register(u2);

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    uint32_t particleIndex = DTid.x;
    if (particleIndex < cParticleSize.s) {
        // Particle構造体の全要素を0で埋めるという書き方
        rParticleData[particleIndex] = (ParticleData)0;
        rFreeList[particleIndex] = particleIndex;
    }
    if (particleIndex == 0) {
        rFreeListIndex[0] = cParticleSize.s - 1;
    }
}