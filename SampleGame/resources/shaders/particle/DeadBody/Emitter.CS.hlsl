#include "../Buffer.hlsli"

const static float32_t positionScalar = 0.6f;
const static float32_t velocityScalar = 8.0f;
const static float32_t scale = 0.03f;

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
    RandomGenerator rand; // 乱数
    rand.seed = (DTid + cPerFrame.time) + cPerFrame.time + 1;
        
    int32_t freeListIndex;
    // FreeListのIndexを1つ前に設定し、現在のIndexを取得する
    InterlockedAdd(rFreeListIndex[0], -1, freeListIndex);
    // 最大数よりもパーティクルの数が少なければ射出可能
    if (0 <= freeListIndex && freeListIndex < cParticleSize.s) {
        uint32_t particleIndex = rFreeList[freeListIndex];
        // パーティクルを生成
        rParticleData[particleIndex].translate = cEmitter.position + (rand.Generate3d() * positionScalar) - float32_t3(positionScalar / 2.0f, positionScalar / 2.0f, positionScalar / 2.0f);
        rParticleData[particleIndex].velocity = (rand.Generate3d() * velocityScalar) - float32_t3(velocityScalar / 2.0f, velocityScalar / 2.0f, velocityScalar / 2.0f);
        rParticleData[particleIndex].scale = float32_t3(scale, scale, scale);
        rParticleData[particleIndex].color = float32_t4(0.463f, 0.592f, 0.318f, 1.0f);
        rParticleData[particleIndex].lifeTime = 0.0f;
        rParticleData[particleIndex].isActive = 1;
        rParticleData[particleIndex].id = particleIndex;
    }
    else {
        // 発生させられなかったので減らしてしまった分もとに戻す。これを忘れると発生させられなかった分だけどんどんIndexがへってしまう
        InterlockedAdd(rFreeListIndex[0], 1);
        // Emit中にParticleは消えないので、この後発生することはないためbreakして終わらせる
    }
}