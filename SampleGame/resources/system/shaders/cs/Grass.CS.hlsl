#include "../ms/UtilityFunction.hlsli"

struct Position
{
    float32_t3 p;
};
RWStructuredBuffer<Position> rPosition : register(u0);

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    RandomGenerator rand;
    rand.seed = float32_t3(DTid);
    
    float32_t2 pos = rand.Generate2d();
    rPosition[DTid.x].p = float32_t3(pos.x, 0.0f, pos.y);
}