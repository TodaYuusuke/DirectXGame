#include "PassThrough.hlsli"

// パラメータ
struct Data
{
    int time; // !< 時間
    int rWidth; // !< 解像度（横幅）
    int rHeight; // !< 解像度（縦幅） 
};
// パラメータ
struct Parameter
{
    //float ; // !< （ブルームを掛ける輝度の閾値）
    float threshold; // !< （ブルームを掛ける輝度の閾値）
};

// パラメータ
ConstantBuffer<Data> gData : register(b0);
ConstantBuffer<Parameter> gPara : register(b1);
// ポストプロセスをかけるテクスチャ（ちゃんとTexture2D二枚で渡せるようにいつか修正する）
// 0 ... 深度マップ
// 1 ... レンダリング結果
Texture2D<float32_t4> gTexture : register(t0);
//Texture2D<float32_t4> gDepth : register(t1);
// サンプラー
SamplerState gSampler : register(s0);

// ガウスブラー
float4 GaussianBlur(float2 uv) {
    float2 texelSize = 1.0f / float2(rWidth, rHeight);

    float blurAmount = sin(50 * 0.02f) * 2;

    float4 blurredColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = -4; i <= 4; ++i)
    {
        for (int j = -4; j <= 4; ++j)
        {
            float2 offset = texelSize * float2(i, j) * blurAmount;
            float4 sampleColor = gTexture.Sample(gSampler, uv + offset);
            // 輝度をチェック
            float brightness = dot(sampleColor.rgb, float3(0.299, 0.587, 0.114));
            // 輝度がしきい値以下なら0をかけて無かったことにする
            float b = step(gPara.threshold, brightness);
            blurredColor += (sampleColor * b);
        }
    }

    // カーネルサイズ 9x9なので81で割る
    return blurredColor / 81.0f;
}


float32_t4 main(PSInput input) : SV_TARGET
{
    // ガウシアンブラー
    return GaussianBlur(input.texcoord);
}