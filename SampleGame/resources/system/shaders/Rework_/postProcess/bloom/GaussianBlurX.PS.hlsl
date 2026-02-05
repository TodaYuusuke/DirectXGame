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
    for (int32_t x = 0; x < kIndexSize; x++) {
        float32_t index = GetIndex(x);
        kernel[x] = Gauss(index, 0.0f, kStandardDeviation);
        weight += kernel[x];
        
        // 現在のtexcoordを算出
        float32_t2 uv = input.texcoord;
        uv.x = input.texcoord.x + index * uvStepSize.x;
        // 色に1/9掛けて足す
        float32_t3 fetchColor = gTexture.Sample(gSampler, uv).rgb;
        output.rgb += fetchColor * kernel[x];
    }
    // 畳み込み後の正規化する
    output.rgb *= rcp(weight);
    
    return output;
}
