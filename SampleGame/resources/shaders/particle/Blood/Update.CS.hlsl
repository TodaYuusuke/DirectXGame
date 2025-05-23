#include "../Buffer.hlsli"

const static float32_t kGravity_ = -9.8f / 100.0f;
const static float32_t kDeleteTime = 3.0f;

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
    uint32_t particleIndex = DTid.x;
    if (particleIndex < cParticleSize.s) {
        // アクティブならば更新
        if (rParticleData[particleIndex].isActive) {
			// 重力を加算
            rParticleData[particleIndex].velocity.y += kGravity_;
			// 速度ベクトルを加算
            rParticleData[particleIndex].translate += rParticleData[particleIndex].velocity * cPerFrame.deltaTime;
			// 速度を弱める
            rParticleData[particleIndex].velocity *= 0.99f;
            
            // 徐々に小さく
            rParticleData[particleIndex].scale = Lerp(
                rParticleData[particleIndex].scale,
                float32_t3(0.0f, 0.0f, 0.0f),
                InCubic(rParticleData[particleIndex].lifeTime / kDeleteTime)
            );
            
            // デルタタイム加算
            rParticleData[particleIndex].lifeTime += cPerFrame.deltaTime;
            // 制限時間経過でアクティブ終了
            if (rParticleData[particleIndex].lifeTime > kDeleteTime) {
                rParticleData[particleIndex].isActive = 0;
                int32_t freeListIndex;
                InterlockedAdd(rFreeListIndex[0], 1, freeListIndex);
                // 最新のFreeListIndexの場所に死んだParticleのIndexを設定する
                if ((freeListIndex + 1) < cParticleSize.s)
                {
                    rFreeList[freeListIndex + 1] = particleIndex;
                }
                else {
                    // ここにくるはずがない、きたら何かが間違っているが、安全策を打っておく
                    InterlockedAdd(rFreeListIndex[0], -1, freeListIndex);
                }
            }

        }
    }
}