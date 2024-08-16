struct CommonData {
    uint32_t instanceSize;
};
struct InstanceData {
    float32_t3 position;
};
struct Camera {
    float32_t4x4 viewProjection; // !< ViewProjection
    float32_t4x4 rotate; // !< RotateMatrix for Billboard
    float32_t3 position; // !< WorldPosition
};

struct PayLoad {
    uint32_t groupID;
};
struct VSOutput {
    float32_t4 pos      : SV_POSITION0;
    float32_t3 normal   : NORMAL0;
    float32_t4 color    : COLOR0;
};

struct Vertex
{
    float4 clipSpacePosition : SV_POSITION;
    float3 worldSpacePosition : POSITION0;
    float3 worldSpaceNormal : NORMAL0;
    float rootHeight : BLENDWEIGHT0;
    float height : BLENDWEIGHT1;
};
struct GrassPatchArguments
{
    float3 position;
    float3 normal;
    float height;
};

void MakePersistentLength(in float3 v0, inout float3 v1, inout float3 v2, in float height)
{
    //Persistent length
    float3 v01 = v1 - v0;
    float3 v12 = v2 - v1;
    float lv01 = length(v01);
    float lv12 = length(v12);

    float L1 = lv01 + lv12;
    float L0 = length(v2 - v0);
    float L = (2.0f * L0 + L1) / 3.0f; //http://steve.hollasch.net/cgindex/curves/cbezarclen.html

    float ldiff = height / L;
    v01 = v01 * ldiff;
    v12 = v12 * ldiff;
    v1 = v0 + v01;
    v2 = v1 + v12;
}
float3 GetWindOffset(float2 pos, float time)
{
    float posOnSineWave = cos(WindDirection) * pos.x - sin(WindDirection) * pos.y;
    
    float t = time + posOnSineWave + 4 * PerlinNoise2D(0.1 * pos);
    float windx = 2 * sin(.5 * t);
    float windy = 1 * sin(1. * t);
    
    return ANIMATION_SCALE * float3(windx, windy, 0);
}

ConstantBuffer<CommonData> cCommon : register(b1);
ConstantBuffer<Camera> cCamera : register(b2);
StructuredBuffer<GrassPatchArguments> sGrassPatch : register(t0);
