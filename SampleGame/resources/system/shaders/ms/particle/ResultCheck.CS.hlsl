
Texture2D<int32_t> tColorBuffer : register(t0);
Texture2D<int32_t> tStencilTexture : register(t1);

RWStructuredBuffer<uint32_t> rHitList : register(u0);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    int32_t stencil = tStencilTexture[DTid.xy]; // テクスチャのデータを取得
    if (stencil != 0) {
        int32_t id = tColorBuffer[DTid.xy];
        if (id != -1) {
            // オブジェクトIDをユニークに保存（Atomic操作）
            InterlockedAdd(rHitList[id], 1);
        }
    }
}