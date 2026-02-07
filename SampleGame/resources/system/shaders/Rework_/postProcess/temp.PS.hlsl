
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

struct RadialBlurData {
	float32_t2 center;
	float32_t blurWidth;
};
ConstantBuffer<RadialBlurData> radialData : register(b1);

float32_t3 RadialBlur(float32_t3 rgb, float32_t2 uv) {
	const int32_t kNumSamples = 10;
	float32_t3 result = rgb;

	float32_t2 direction = uv - radialData.center;
	for(int32_t sampleIndex = 1; sampleIndex < kNumSamples; sampleIndex++) {
		float32_t2 texcoord = uv + direction * radialData.blurWidth * float32_t(sampleIndex);
		result += gTexture.Sample(gSampler, texcoord).rgb;
	}

	return result * rcp(kNumSamples);
}
float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);

	output.rgb = RadialBlur(output.rgb, uv);

	output.a = 1.0f;
	return output;
}
