#include "Object3d.hlsli"

float main(VertexShaderOutput input) : SV_TARGET{
    // デプス値の計算
    float depth = input.position.z / input.position.w;
    return output;
}