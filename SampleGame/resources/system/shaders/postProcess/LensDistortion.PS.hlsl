#include "PassThrough.hlsli"

// パラメータ
struct Parameter {
	float32_t2 multiplier;	// !< 影響度合い（0.0f ~ 1.0f）
	float32_t2 center;	    // !< 歪みの中心（アンカーポイント）
	float scale;            // !< レンズの拡大率（0.01f ~ 5.0f）
    int32_t intensity;      // !< 強度（-100 ~ 100）
};

// パラメータのバインド
ConstantBuffer<Parameter> gPara : register(b0);
// ポストプロセスをかけるテクスチャ
Texture2D<float32_t4> gTexture : register(t0);
// サンプラー
SamplerState gSampler : register(s0);

float WhiteNoise(float2 coord) {
	return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
}

float32_t4 main(PSInput input) : SV_TARGET{
    // 最終結果
	float32_t4 output;
	
	//output = gTexture.Sample(gSampler, input.texcoord);
    //output = float4(output.r, output.r, output.r, output.a);

	// 元の色を受け取る
	output = gTexture.Sample(gSampler, input.texcoord);
	float noise = WhiteNoise(input.texcoord * gPara.intensity) - 0.5;
	output.rgb += float3(noise, noise, noise);


    return output;
}