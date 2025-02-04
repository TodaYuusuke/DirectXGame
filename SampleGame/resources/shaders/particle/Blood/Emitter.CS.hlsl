#include "Buffer.hlsli"

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
    if (cEmitter.emit != 0) {   // 射出許可が出たので射出
        RandomGenerator rand; // 乱数
        rand.seed = (DTid + cPerFrame.time) + cPerFrame.time;
        
        // カウント分Particleを射出
        for (uint32_t countIndex = 0; countIndex < cEmitter.count; countIndex++) {
            int32_t freeListIndex;
            // FreeListのIndexを1つ前に設定し、現在のIndexを取得する
            InterlockedAdd(rFreeListIndex[0], -1, freeListIndex);
            // 最大数よりもパーティクルの数が少なければ射出可能o
            if (0 <= freeListIndex && freeListIndex < cParticleSize.s) {
                uint32_t particleIndex = rFreeList[freeListIndex];
                // パーティクルを生成
                rParticleData[particleIndex].translate = cEmitter.position;
                rParticleData[particleIndex].velocity = rand.Generate3d() - float32_t3(0.5f, 0.5f, 0.5f);
                rParticleData[particleIndex].scale = float32_t3(0.05f, 0.05f, 0.05f);
                rParticleData[particleIndex].color = float32_t4(1.0f, 0.0f, 0.0f, 1.0f);
                rParticleData[particleIndex].lifeTime = 0.0f;
                rParticleData[particleIndex].isActive = 1;
            }
            else {
                // 発生させられなかったので減らしてしまった分もとに戻す。これを忘れると発生させられなかった分だけどんどんIndexがへってしまう
                InterlockedAdd(rFreeListIndex[0], 1);
                // Emit中にParticleは消えないので、この後発生することはないためbreakして終わらせる
                break;
            }
        }
    }
}