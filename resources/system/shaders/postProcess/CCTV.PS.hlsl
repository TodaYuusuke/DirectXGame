#include "PassThrough.hlsli"

// パラメータ
struct Parameter {
	int time;	// !< 時間
    int rWidth;	// !< 解像度（横幅）
    int rHeight; // !< 解像度（縦幅） 
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
/*float SSAO(float2 uv) {
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
}*/
float CheckDepth(float a, float b, float c) {
    // aとcの中間のデータ - 中心の深度値が大きすぎなければ1を返す
    if ((b < a && b < c) || (b > a && b < c) || (b < a && b > c))
    {
        return smoothstep(0.0000002f, 0.0001f, (b - a) + (b - c));
        //return step(0.0000002f, (b - a) + (b - c));
    }
    return 0;
    // なんかアウトラインになった
    //if ((a + b + c) == b * 3.0f) { return 0; }
    //return step((b - a) + (b - c), -0.0002f);
    
    //return step(b * 2, a + c - (abs(a - c) / 2.0f));
    //return step(b, a) * step(b, c);
	
}
float Sampling(float2 uv, float depth, float x, float y)
{
    return CheckDepth(
		gTexture[0].Sample(gSampler, float2(uv.x + x, uv.y + y)).r,
		depth,
		gTexture[0].Sample(gSampler, float2(uv.x - x, uv.y - y)).r
	);
}
float SSAO(float2 uv)
{
	// サンプリングする外周の数
    const uint kSampleCount = 20;
    const uint kSkipPixel = 2; // 飛ばす距離
	// 一ピクセル分の値
    const float kWidth = 1.0f / gPara.rWidth;
    const float kHeight = 1.0f / gPara.rHeight;
	// 中央の深度値
    float cDepth = gTexture[0].Sample(gSampler, uv).r;
	// 結果
    float result = 0.0f;
	// サンプリングした数
    int sampleCount = 0;
	
	// 十字の部分を担当
    for (int n = 1; n <= kSampleCount; n += kSkipPixel)
    {
        result += Sampling(uv, cDepth, n * kWidth, 0); // 右
        result += Sampling(uv, cDepth, -n * kWidth, 0); // 左
        result += Sampling(uv, cDepth, 0, n * kHeight); // 上
        result += Sampling(uv, cDepth, 0, -n * kHeight); // 下
        sampleCount += 4;
    }
	// 角を担当
    for (int y = 1; y <= kSampleCount; y += kSkipPixel)
    {
        for (int x = 1; x <= kSampleCount; x += kSkipPixel)
        {
            result += Sampling(uv, cDepth, x * kWidth, y * kHeight);
            result += Sampling(uv, cDepth, -x * kWidth, y * kHeight);
            sampleCount += 2;
        }
    }
	
    return 1.0f - (result / sampleCount);
}

// なんかSSAO作ってたはずなのにOutLineになっちゃった(´・ω・｀)
float OutLine(float2 uv) {
	// サンプリングする外周の数
    const uint kSampleCount = 10;
    const uint kSkipPixel = 1;	// 飛ばす距離
	// 一ピクセル分の値
    const float kWidth = 1.0f / gPara.rWidth;
    const float kHeight = 1.0f / gPara.rHeight;
	// 中央の深度値
    float cDepth = gTexture[0].Sample(gSampler, uv).r;
	// 結果
    float result = 0.0f;
	// サンプリングした数
    int sampleCount = 0;
	
	// 十字の部分を担当
    for (int n = 1; n <= kSampleCount; n+=kSkipPixel) {
        result += Sampling(uv, cDepth, n * kWidth, 0); // 右
        result += Sampling(uv, cDepth, -n * kWidth, 0); // 左
        result += Sampling(uv, cDepth, 0, n * kHeight); // 上
        result += Sampling(uv, cDepth, 0, -n * kHeight); // 下
        sampleCount += 4;
    }
	// 角を担当
    for (int y = 1; y <= kSampleCount; y+=kSkipPixel) {
        for (int x = 1; x <= kSampleCount; x+=kSkipPixel) {
            result += Sampling(uv, cDepth, x * kWidth, y * kHeight);
            result += Sampling(uv, cDepth, -x * kWidth, y * kHeight);
			sampleCount += 2;
        }
    }
	
    return 1.0f - (result / sampleCount);
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
    output.rgb *= SSAO(uv);
	// 走査線
	output.rgb += ScanLine(uv);
	// ビネット（ビネットは画面自体へついてほしいので元のUVを使う）
	output.rgb -= Vignette(input.texcoord);

	return output;
}