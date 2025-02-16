#include "Buffer.hlsli"

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
	uint32_t particleIndex = DTid.x;
	if (particleIndex < cParticleSize.s) {
	// ヒットしているならば処理を行う
		if (sHitList[particleIndex] > 0) {
			// ぶつかったら反転する
            float32_t3 v = rParticleData[particleIndex].velocity;
            v.x *= -0.5f;
            v.z *= -0.5f;
            rParticleData[particleIndex].velocity = v;
        }
	}
}