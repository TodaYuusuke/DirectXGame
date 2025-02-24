#include "../Buffer.hlsli"

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
	uint32_t particleIndex = DTid.x;
	if (particleIndex < cParticleSize.s) {
	// ヒットしているならば処理を行う
		if (sHitList[particleIndex] > 0) {
			// ぶつかったら反転する
            rParticleData[particleIndex].velocity.y *= -0.99f;
            rParticleData[particleIndex].color = float32_t4(0.0f, 1.0f, 0.0f, 1.0f);
        }
	}
}