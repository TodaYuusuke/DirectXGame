#include "../ms/UtilityFunction.hlsli"

struct AABB
{
    float32_t3 min;
    float32_t3 max;
};
struct Position
{
    float32_t3 p;
};
ConstantBuffer<AABB> cAABB : register(b0);
RWStructuredBuffer<Position> rPosition : register(u0);
Texture2D<float32_t4> tTexture : register(t0);
SamplerState sSampler : register(s0);

// 草を生やす地点の色
const static float32_t3 kGrassColor = float32_t3(0.1686f, 0.7059f, 0.0863f);
const static float32_t kThreshold = 0.001f;
const static uint32_t kMaxLoop = 1000;

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    RandomGenerator rand;
    rand.seed = float32_t3(DTid);
    
    //// 地点を生成
    //float32_t2 pos = rand.Generate2d();
    //// サンプリングした地点の色と特定の色の差を計算
    //float32_t colorDifference = distance(tTexture.Sample(sSampler, pos).rgb, kGrassColor);
    
    //// 色が近いかどうかの判定
    //if (step(colorDifference, kThreshold)) {
    //    rPosition[DTid.x].p = float32_t3(
    //        cAABB.min.x + distance(cAABB.min.x, cAABB.max.x) * pos.x,
    //        0.0f,
    //        cAABB.min.z + distance(cAABB.min.z, cAABB.max.z) * pos.y
    //    );
    //}
    
    // 特定の色が見つかるまでループするver
    for (uint32_t i = 0; i < kMaxLoop; i++)
    {
        // 地点を乱数生成
        float32_t2 pos = rand.Generate2d();
        // サンプリングした地点の色と特定の色の差を計算
        float32_t colorDifference = distance(tTexture.Sample(sSampler, pos).rgb, kGrassColor);
        
        // 差が閾値以下なら地点を生成して終了
        if (colorDifference < kThreshold) {
            // 草を生やす座標を生成
            rPosition[DTid.x].p = float32_t3(
                cAABB.min.x + distance(cAABB.min.x, cAABB.max.x) * pos.x,
                0.0f,
                cAABB.min.z + distance(cAABB.min.z, cAABB.max.z) * pos.y
            );
            break;
        }
    };
    
        
}