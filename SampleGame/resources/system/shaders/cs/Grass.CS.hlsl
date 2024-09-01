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
const static float32_t kThreshold = 0.3f;
const static uint32_t kMaxLoop = 1000;

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    RandomGenerator rand;
    rand.seed = float32_t3(DTid + (1,1,1));
    
    // 単純にランダム生成
    /*float32_t2 pos = rand.Generate2d();
    rPosition[DTid.x].p = float32_t3(
                pos.x * 3.0f,
                0.0f,
                pos.y * 3.0f
    );*/
    
    // 定の色が見つかるまでループするver
    for (uint32_t i = 0; i < kMaxLoop; i++)
    {
        // 地点を乱数生成
        float32_t2 pos = rand.Generate2d();
        // サンプリングした地点の色と特定の色の差を計算
        float32_t3 color = tTexture.Sample(sSampler, pos).rgb;
        //float32_t colorDifference = sqrt(
        //    pow(kGrassColor.r - color.r, 2) +
        //    pow(kGrassColor.g - color.g, 2) +
        //    pow(kGrassColor.b - color.b, 2)
        //);
        float32_t colorDifference = length(color - kGrassColor);
        
        // 差が閾値以下なら地点を生成して終了
        if (colorDifference <= kThreshold)
        {
            // 草を生やす座標を生成
            rPosition[DTid.x].p = float32_t3(
                cAABB.min.x + ((cAABB.max.x - cAABB.min.x) * pos.x),
                0.0f,
                cAABB.min.z + ((cAABB.max.z - cAABB.min.z) * (1.0f - pos.y))
            );
            //rPosition[DTid.x].p = float32_t3(
            //    pos.x * 3.0f,
            //    0.0f,
            //    (1.0f - pos.y) * 3.0f
            //);
            break;
        }
    };
    
        
}