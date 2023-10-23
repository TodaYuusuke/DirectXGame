#include "ShadowMap.hlsli"

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.position = mul(mul(input.position, gWorldMatrix.World), gDirectionalLight.viewProjection);
    return output;
}