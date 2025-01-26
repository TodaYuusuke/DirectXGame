#include "Buffer.hlsli"


[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
    RandomGenerator rand; // 乱数
    rand.seed = (DTid + cPerFrame.time) + cPerFrame.time;
    
    if (cEmitter.emit != 0) {   // 射出許可が出たので射出
        for (uint32_t countIndex = 0; countIndex < cEmitter.count; countIndex++) {
            // カウント分Particleを射出
            rParticleData[countIndex].translate = 
                cEmitter.position + (rand.Generate3d() * cEmitter.radius);
            rParticleData[countIndex].velocity = rand.Generate3d();
            rParticleData[countIndex].scale = rand.Generate3d();
            rParticleData[countIndex].lifeTime = 0.0f;
        }
    }
}