#include "Object3d.hlsli"

float32_t4 main(VertexShaderOutput input) : SV_TARGET {
    float32_t4 output;

    if (gMaterial.enableLighting != 0) { // Lightingの計算を行う
        // シェーディング
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        // uvTransform
        float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
        // シャドウマッピング
        float sm = gShadowMap.Sample(gShadowMapSampler, input.posSM.xy);
        float sma = (input.posSM.z - 0.005f < sm) ? 1.0f : 0.5f;
        
        output.rgb = input.color.rgb * gTexture.Sample(gSampler, transformUV.xy).rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity * sma;
        output.w = input.color.a * gTexture.Sample(gSampler, transformUV.xy).a;   // 透明度を保持
    }
    else { // Lightingの計算を行わない
        float4 transformUV = mul(float32_t4(input.texcoord,0.0f,1.0f), gMaterial.uvTransform);
        output = input.color * gTexture.Sample(gSampler, transformUV.xy);
    }
    return output;
}