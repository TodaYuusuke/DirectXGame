#include "Plane.hlsli"

float32_t4 main(VSOutputUnlit input) : SV_TARGET
{
    // マテリアルを抽出
    Material m = pMaterial[input.id];
    
    // 最終的な出力
    float32_t4 output;
    
    // Lightingの計算は行わない
    float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), m.uvTransform);
    output = input.color * pTexture[m.tIndex].Sample(pSampler, transformUV.xy) * m.color;

    // 透明なら消す
    if (output.a <= 0.0f) { discard; }

    return output;
}