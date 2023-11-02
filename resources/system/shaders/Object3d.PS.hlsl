#include "Object3d.hlsli"

float32_t4 main(VertexShaderOutput input) : SV_TARGET{
    // インデックスを抽出
    uint32_t m = gIndex[input.id].material;
    uint32_t t = gIndex[input.id].tex2d;


    // 最終的な結果
    float32_t4 output;

    if (gMaterial[m].enableLighting != 0) { // Lightingの計算を行う
        // シェーディング
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        // uvTransform
        float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial[m].uvTransform);
        // シャドウマッピング
        float sm = gShadowMap.Sample(gShadowMapSampler, input.posSM.xy);
        float sma = (input.posSM.z - 0.0000004f < sm) ? 1.0f : 0.5f;
        //float sma = (input.posSM.z - 0.0000005f < sm) ? 1.0f : 0.5f;
        //float sma = (input.posSM.z - 0.000025f < sm) ? 1.0f : 0.5f;

        output.rgb = input.color.rgb * gTexture[t].Sample(gSampler, transformUV.xy).rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity * sma;
        output.w = input.color.a * gTexture[t].Sample(gSampler, transformUV.xy).a;   // 透明度を保持
    }
    else { // Lightingの計算を行わない
        float4 transformUV = mul(float32_t4(input.texcoord,0.0f,1.0f), gMaterial[m].uvTransform);
        output = input.color * gTexture[t].Sample(gSampler, transformUV.xy);
    }
    return output;
}