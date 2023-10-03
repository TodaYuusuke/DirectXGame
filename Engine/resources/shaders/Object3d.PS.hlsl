#include "Object3d.hlsli"

float32_t4 main(VertexShaderOutput input) : SV_TARGET {
    float32_t4 output;

    if (gMaterial.enableLighting != 0) { // Lightingの計算を行う
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
        output = input.color * gTexture.Sample(gSampler, transformUV.xy) * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        output.w = input.color.w;   // 透明度を保持
    }
    else { // Lightingの計算を行わない
        float4 transformUV = mul(float32_t4(input.texcoord,0.0f,1.0f), gMaterial.uvTransform);
        output = input.color * gTexture.Sample(gSampler, transformUV.xy);
    }
    return output;
}