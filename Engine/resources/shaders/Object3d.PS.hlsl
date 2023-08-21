#include "Object3d.hlsli"

float32_t4 main(VertexShaderOutput input) : SV_TARGET {
    float32_t4 output;

    if (gMaterial.enableLighting != 0) { // Lightingの計算を行う
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        //float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        output = input.color * gTexture.Sample(gSampler, input.texcoord) * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    }
    else { // Lightingの計算を行わない
        output = input.color * gTexture.Sample(gSampler, input.texcoord);
    }
    return output;
}