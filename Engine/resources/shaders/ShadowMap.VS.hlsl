#include "Object3d.hlsli"

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.position = mul(mul(input.position, gWorldMatrix.World), gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)gWorldMatrix.World));
    output.color = input.color;
    return output;
}