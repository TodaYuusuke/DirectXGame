#include "Solid.hlsli"

float32_t4 main(MSOutput input) : SV_TARGET
{
    // インデックス抽出
    int32_t m = input.mIndex;
    int32_t t = iMaterials[m].tIndex;
    
    // 最終的な出力結果
    float32_t4 output = { 0.0f, 0.0f, 0.0f, 0.0f };
    
    float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), iMaterials[m].uvTransform);
    output = input.color * cTexture[t].Sample(cTexSmp, transformUV.xy) * iMaterials[m].color;
    
    // 計算結果を返す
    return output;
}