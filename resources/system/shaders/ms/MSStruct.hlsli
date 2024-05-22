#include "../Struct.hlsli"

struct ModelData
{
    WorldTransform wtf;
    int32_t enableLighting;
};

struct ConstantData
{
    float32_t4x4 WorldViewProj;
    uint32_t     DrawMeshlets;
};

struct Meshlet
{
    uint32_t VertCount;
    uint32_t VertOffset;
    uint32_t PrimCount;
    uint32_t PrimOffset;
};

struct VertexOutPut
{
    float32_t4 pos      : SV_POSITION0;
    float32_t3 worldPos : POSITION0;
    float32_t2 texCoord : TEXCOORD0;
    float32_t3 normal   : NORMAL0;
    float32_t4 color    : COLOR0;
    int32_t mIndex : INDEX0;
};