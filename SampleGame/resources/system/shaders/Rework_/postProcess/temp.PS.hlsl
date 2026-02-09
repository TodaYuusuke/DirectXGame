
#include "../utility/Random.hlsli"

struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};
struct Parameter
{
    float time;
    int rWidth;
    int rHeight;
};

ConstantBuffer<Parameter> gPara : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gDepth : register(t1);
SamplerState gSampler : register(s0);
SamplerState gPointSampler : register(s1);

struct FogData {
	float32_t4x4 projectionInverse;
	float32_t4 color;
	float32_t fogNear;
	float32_t fogFar;
};
ConstantBuffer<FogData> fogData : register(b1);

float32_t3 ApplyFog(float32_t2 uv, float32_t3 color) {
	float32_t ndcDepth = gDepth.Sample(gPointSampler, uv);
	
	// NDCからビュー空間へ変換（Zのみ）
	float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), fogData.projectionInverse);
	float32_t viewZ = viewSpace.z * rcp(viewSpace.w);
	
	// 霧の係数を計算（線形）
	float32_t fogFactor = saturate((viewZ - fogData.fogNear) / (fogData.fogFar - fogData.fogNear));
	fogFactor *= fogData.color.a;	// アルファ値を強度として使用
	
	return lerp(color, fogData.color.rgb, fogFactor);
}

float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);

	output.rgb = ApplyFog(uv, output.rgb);

	output.a = 1.0f;
	return output;
}
