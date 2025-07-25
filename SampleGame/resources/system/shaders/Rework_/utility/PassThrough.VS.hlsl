#include "../Structs.hlsli"

static const uint32_t kNumVertex = 3;
static const float32_t4 kPositions[kNumVertex] = {
    { -1.0f, 1.0f, 0.0f, 1.0f },
    { 3.0f, 1.0f, 0.0f, 1.0f },
    { -1.0f, -3.0f, 0.0f, 1.0f },
};
static const float32_t2 kTexcoords[kNumVertex] = {
    { 0.0f, 0.0f },
    { 2.0f, 0.0f },
    { 0.0f, 2.0f },
};

// 頂点データを必要としない（フルスクリーンレンダリング前提の）PixelShader用のVertexShader
PassThroughOutput main(uint32_t vertexID : SV_VertexID) {
    PassThroughOutput output;
    output.position = kPositions[vertexID];
    output.texcoord = kTexcoords[vertexID];
    return output;
}