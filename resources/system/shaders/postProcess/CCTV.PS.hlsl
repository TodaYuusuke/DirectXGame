#include "PassThrough.hlsli"

// パラメータ
struct Parameter {
	int time;	// !< 時間
};

// パラメータのバインド
ConstantBuffer<Parameter> gPara : register(b0);
// ポストプロセスをかけるテクスチャ（ちゃんとTexture2D二枚で渡せるようにいつか修正する）
// 0 ... 深度マップ
// 1 ... レンダリング結果
Texture2D<float32_t4> gTexture[] : register(t0);
//Texture2D<float32_t4> gDepth : register(t1);
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


// 半径内のランダムな点をサンプリングする関数
float2 RandomPointInRadius(float2 center, float radius, uint n) {
    // ランダムな角度を生成する
    float angle = WhiteNoise(float2(center.x + n, center.y + n)); // ホワイトノイズからランダムな角度を生成
    // ランダムな距離を生成する
    float distance = WhiteNoise(float2(center.x + n + 1.0f, center.y + n + 1.0f)); // パーリンノイズからランダムな距離を生成
    distance = distance * radius; // 0からradiusの間のランダムな距離に変換
    // 中心からのオフセットを計算する
    float xOffset = distance * cos(angle);
    float yOffset = distance * sin(angle);
    // 新しい点の座標を返す
    return float2(center.x + xOffset, center.y + yOffset);
}
// SSAO
float SSAO(float2 uv) {
	// サンプリングの数
    const uint kSampleCount = 15;
	// 中央の深度値
    float centerDepth = gTexture[0].Sample(gSampler, uv).r;
	// 結果
    float result = 0.0f;
	
    for (uint n = 0; n < kSampleCount; n++)
	{
		// ランダムな地点からサンプリングした深度値
        float sampleDepth = gTexture[0].Sample(gSampler, RandomPointInRadius(uv, 0.00001f, n)).r;
		// 中央との深度値の差が大きい場合は1を返す
        //result += 1.0f - step(0.001f, abs(centerDepth - sampleDepth));
        result += step(centerDepth, sampleDepth);
    }
    return result / kSampleCount;
}

float32_t4 main(PSInput input) : SV_TARGET {
	// 最終結果
	float32_t4 output;
    output.rgb = float3(1.0f, 1.0f, 1.0f);
    output.a = 1.0f;
	// レンダリングする座標
	float2 uv = input.texcoord;

	// 樽状彎曲させる
	uv = LensDistortion(uv);
	// サンプリング
    output = gTexture[1].Sample(gSampler, uv);
	
	// SSAO
    //output.rgb *= SSAO(uv);
	// 走査線
	output.rgb += ScanLine(uv);
	// ビネット（ビネットは画面自体へついてほしいので元のUVを使う）
	output.rgb -= Vignette(input.texcoord);

	return output;
}