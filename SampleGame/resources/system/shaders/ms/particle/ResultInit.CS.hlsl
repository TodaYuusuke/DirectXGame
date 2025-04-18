#include "../../utility/Particle.hlsli"

ConstantBuffer<ParticleSize> cParticleSize : register(b0);
RWStructuredBuffer<uint32_t> rHitList : register(u0);

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    uint32_t particleIndex = DTid.x;
    if (particleIndex < cParticleSize.s) {
        rHitList[particleIndex] = 0;
    }
}