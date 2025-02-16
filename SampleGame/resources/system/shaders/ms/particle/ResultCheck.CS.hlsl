
Texture2D<int32_t> tColorBuffer : register(t0);
Texture2D<float32_t> tDepthStencil : register(t1);

RWStructuredBuffer<uint32_t> rHitList : register(u0);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    float32_t srcValue = tDepthStencil[DTid.xy]; // R32_FLOATデータを取得
    uint32_t raw = asuint(srcValue); // float を uint32_t に変換
    uint32_t stencil = raw & 0xFF; // 下位 8 ビットのステンシル値を抽出
    if (stencil != 0) {
        int32_t id = tColorBuffer[DTid.xy];
        if (id != -1) {
            // オブジェクトIDをユニークに保存（Atomic操作）
            InterlockedAdd(rHitList[id], 1);
        }
    }
}