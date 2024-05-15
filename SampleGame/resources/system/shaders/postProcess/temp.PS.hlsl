
struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct Parameter
{
    int time;
    int rWidth;
    int rHeight;
};

ConstantBuffer<Parameter> gPara : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t4> gDepth : register(t1);
SamplerState gSampler : register(s0);

struct GrayScaleData {
	float32_t intensity;
};
ConstantBuffer<GrayScaleData> gsData : register(b1);

float32_t3 GrayScale(float32_t3 color) {
    float32_t3 result = dot(color, float32_t3(0.2125f, 0.7154f, 0.0721f));
    return lerp(color, result, gsData.intensity);
}

struct VignettingData {
	float32_t intensity;
};
ConstantBuffer<VignettingData> vData : register(b2);

float32_t3 Vignetting(float32_t2 uv, float32_t3 color) {
	float32_t2 correct = uv * (1.0f - uv.yx);
	float vignette = correct.x * correct.y * 16.0f;
	vignette = saturate(pow(vignette, 0.8f));
	return color * vignette;
}

float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);

	output.rgb = GrayScale(output.rgb);

	output.rgb = Vignetting(uv, output.rgb);

	return output;
}
