#include "Object3d.hlsli"

float32_t4 main(VertexShaderOutput input) : SV_TARGET{
    float32_t4 output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output = input.color * textureColor;
    //output = input.color;
    return output;
}