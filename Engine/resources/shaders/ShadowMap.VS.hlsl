#include "Object3d.hlsli"

float4 main(VertexShaderInput input) {
    float32_t4 position;
    position = mul(mul(input.position, gWorldMatrix.World));
    return output;
}