
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
Texture2D<float32_t> gDepth : register(t1);
SamplerState gSampler : register(s0);
SamplerState gPointSampler : register(s1);

struct RGBShiftData {
	float32_t2 shiftR;
	float32_t2 shiftG;
	float32_t2 shiftB;
};
ConstantBuffer<RGBShiftData> rgbShiftData : register(b1);

float32_t3 RGBShift(float32_t2 uv) {
    float32_t r = gTexture.Sample(gSampler, uv + rgbShiftData.shiftR).r;
    float32_t g = gTexture.Sample(gSampler, uv + rgbShiftData.shiftG).g;
    float32_t b = gTexture.Sample(gSampler, uv + rgbShiftData.shiftB).b;
    return float32_t3(r, g, b);
}

float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);

	output.rgb = RGBShift(uv);

	output.a = 1.0f;
	return output;
}
