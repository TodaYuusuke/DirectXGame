#include "GaussianBlur.hlsli"

float32_t4 main(PSInput input) : SV_TARGET {
    float32_t weight = 0.0f;
    float32_t kernel[kIndexSize];
    // BoxFilter
    uint32_t width, height; // 1. uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
    // 出力カラー
    float32_t4 output = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int32_t y = 0; y < kIndexSize; y++) {
        float32_t index = GetIndex(y);
        kernel[y] = Gauss(0.0f, index, kStandardDeviation);
        weight += kernel[y];
        
        // 現在のtexcoordを算出
        float32_t2 uv = input.texcoord;
        uv.y = input.texcoord.y + index * uvStepSize.y;
        // 色に1/9掛けて足す
        float32_t3 fetchColor = gTexture.Sample(gSampler, uv).rgb;
        output.rgb += fetchColor * kernel[y];
    }
    // 畳み込み後の正規化する
    output.rgb *= rcp(weight);
    
    return output;
}
