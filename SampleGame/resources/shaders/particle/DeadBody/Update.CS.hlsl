#include "Buffer.hlsli"

const static uint32_t kMaxParticles = 1024;

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
    uint32_t particleIndex = DTid.x;
    if (particleIndex < kMaxParticles) {
        if (rParticleData[particleIndex].lifeTime <= 180.0f) {
            rParticleData[particleIndex].translate += rParticleData[particleIndex].velocity;
            rParticleData[particleIndex].lifeTime += cPerFrame.deltaTime;
        }
    }
}