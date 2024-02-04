#include "PassThrough.hlsli"

// パラメータ
struct Parameter {
	int time;	// !< 時間
};

// パラメータのバインド
ConstantBuffer<Parameter> gPara : register(b0);
// ポストプロセスをかけるテクスチャ
Texture2D<float32_t4> gTexture : register(t0);
// 深度マップ
Texture2D<float> gDepth : register(t1);
// サンプラー
SamplerState gSampler : register(s0);

// ホワイトノイズ
float WhiteNoise(float2 coord) {
	return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
}

// 走査線
float3 ScanLine(float2 uv) {
	float time = gPara.time;
	float3 result = float3(0.0, 0.0, 0.0);
	float sinv = sin(uv.y * 2 + time * -0.01);
	float steped = step(0.99, sinv * sinv);
	result.rgb -= (1 - steped) * abs(sin(uv.y * 50.0 + time * 1.0)) * 0.05;
	result.rgb -= (1 - steped) * abs(sin(uv.y * 100.0 - time * 2.0)) * 0.08;
	result.rgb += steped * 0.1;
	return result;
}
// 樽状彎曲
float2 LensDistortion(float2 uv) {
	uv -= float2(0.5, 0.5);
	float distPower = pow(length(uv), 0.1);
	uv *= float2(distPower,distPower);
	uv += float2(0.5 , 0.5);
	return uv;
}
// ビネット
float Vignette(float2 uv) {
	float vignette = length(float2(0.5,0.5) - uv);
	vignette = clamp(vignette - 0.2, 0, 1);
	return vignette;
}

float32_t4 main(PSInput input) : SV_TARGET{
	// 最終結果
	float32_t4 output;
	output.a = 1.0f;
	// レンダリングする座標
	float2 uv = input.texcoord;

	// 樽状彎曲させる
	uv = LensDistortion(uv);
	// サンプリング
	output = gTexture.Sample(gSampler, uv);
	
	// 走査線
	output.rgb += ScanLine(uv);
	// ビネット（ビネットは画面自体へついてほしいので元のUVを使う）
	output.rgb -= Vignette(input.texcoord);

	return output;
}