
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

struct OutLineData {
	float32_t4x4 projectionInverse;
};
ConstantBuffer<OutLineData> olData : register(b1);

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

float32_t OutLine(float32_t2 uv) {
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
	return (1.0f - weight);
}

float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);

	output.rgb *= OutLine(uv);

	return output;
}
