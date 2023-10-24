#include "Object3d.hlsli"

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.position = mul(mul(input.position, gWorldMatrix.World), gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)gWorldMatrix.World));
    output.color = input.color;

    float4 Pos = mul(mul(input.position, gWorldMatrix.World), gDirectionalLight.viewProjection);
    Pos.xyz = Pos.xyz / Pos.w;
    output.posSM.x = (1.0f + Pos.x) / 2.0f;
    output.posSM.y = (1.0f - Pos.y) / 2.0f;
    output.posSM.z = Pos.z;

    return output;
}