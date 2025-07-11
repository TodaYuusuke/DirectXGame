#include "Solid.hlsli"

GBufferOutput main(MSOutput input) {
    // インデックス抽出
    int32_t m = input.mIndex;
    int32_t t = iMaterials[m].tIndex;
    
    // 最終的な出力結果
    GBufferOutput output;
    
    // アルベド出力
    float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), iMaterials[m].uvTransform);
    output.baseColor = input.color * cTexture[t].Sample(cTexSmp, transformUV.xy) * iMaterials[m].color;
    
    // 法線出力
    output.normal.xyz = normalize(EncodeNormal(input.normal));
    output.normal.w = iMaterials[m].enableLighting; // ライティングの有効フラグを法線の空きスペースを使って送る
    
    // マテリアル出力
    //output.pbr.r = iMaterials[m].shinines;
    output.pbr.r = 0.0f;
    output.pbr.g = 0.0f;
    output.pbr.b = 0.0f;
    output.pbr.a = 0.0f;
    
    // ワールド座標出力
    output.worldPosition.xyz = input.worldPos;
    output.worldPosition.w = 1.0f;
    
    // 計算結果を返す
    return output;
}
