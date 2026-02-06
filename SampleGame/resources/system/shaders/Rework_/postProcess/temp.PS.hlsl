
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

struct VignettingData {
	float32_t intensity;
};
ConstantBuffer<VignettingData> vData : register(b1);

float32_t Vignetting(float32_t2 uv) {
	float32_t2 correct = uv * (1.0f - uv.xy);
	float vignette = correct.x * correct.y * 16.0f;
	vignette = saturate(pow(vignette, vData.intensity));
	return vignette;
}
struct OutLineData {
	float32_t4x4 projectionInverse;
	float32_t4 color;
	float32_t threshold;
};
ConstantBuffer<OutLineData> olData : register(b2);

static const float32_t2 kIndex3x3[3][3] = {
	{{-1.0f,-1.0f},{ 0.0f,-1.0f},{ 1.0f,-1.0f}},
	{{-1.0f, 0.0f},{ 0.0f, 0.0f},{ 1.0f, 0.0f}},
	{{-1.0f, 1.0f},{ 0.0f, 1.0f},{ 1.0f, 1.0f}},
};
static const float32_t kPrewittHorizontalKernel[3][3] = {
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};
static const float32_t kPrewittVerticalKernel[3][3] = {
	{ -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

float32_t3 OutLine(float32_t2 uv, float32_t3 color) {
	float32_t2 difference = float32_t2(0.0f, 0.0f);
	uint32_t width, height;
	gTexture.GetDimensions(width, height);
	float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

	for(int32_t x = 0; x < 3; x++) {
		for(int32_t y = 0; y < 3; y++) {
			float32_t2 texcoord = uv + kIndex3x3[x][y] * uvStepSize;
			float32_t ndcDepth = gDepth.Sample(gPointSampler, texcoord);

			float32_t4 viewSpace = mul(float32_t4(0.0f,0.0f,ndcDepth,1.0f), olData.projectionInverse);
			float32_t viewZ = viewSpace.z * rcp(viewSpace.w);
			difference.x += viewZ * kPrewittHorizontalKernel[x][y];
			difference.y += viewZ * kPrewittVerticalKernel[x][y];
		}
	}

	float32_t weight = length(difference);
	weight = saturate(weight);

	if (olData.threshold >= weight) {
		return color;
	}
	return lerp(color, olData.color.rgb, weight);

	// color 元の色
	// olData.color.rgb アウトラインの色
	//return color * (1.0f - weight);
	//return color * (olData.color.rgb * (1.0f - weight));
}

struct GrayScaleData {
	float32_t intensity;
};
ConstantBuffer<GrayScaleData> gsData : register(b3);

float32_t3 GrayScale(float32_t3 color) {
    float32_t3 result = dot(color, float32_t3(0.2125f, 0.7154f, 0.0721f));
    return lerp(color, result, gsData.intensity);
}

float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);

	output.rgb *= Vignetting(uv);

	output.rgb = OutLine(uv, output.rgb);

	output.rgb = GrayScale(output.rgb);

	output.a = 1.0f;
	return output;
}
