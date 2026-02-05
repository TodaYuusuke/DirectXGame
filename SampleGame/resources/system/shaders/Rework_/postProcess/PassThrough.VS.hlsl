// 頂点データを必要としない（フルスクリーンレンダリング前提の）PixelShader用のVertexShader
#include "PassThrough.hlsli"

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

PSInput main(uint32_t vertexID : SV_VertexID) {
    PSInput output;
    output.position = kPositions[vertexID];
    output.texcoord = kTexcoords[vertexID];
    return output;
}