struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct Data {
    float32_t threshold;
};

ConstantBuffer<Data> gData : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const float32_t Epsilon = 1e-10;
float32_t3 RGBtoHCV(float32_t3 RGB) {
    float32_t4 P = (RGB.g < RGB.b) ? float32_t4(RGB.bg, -1.0f, 2.0f / 3.0f) : float32_t4(RGB.gb, 0.0f, -1.0f / 3.0f);
    float32_t4 Q = (RGB.r < P.x) ? float32_t4(P.xyw, RGB.r) : float32_t4(RGB.r, P.yzx);
    float C = Q.x - min(Q.w, Q.y);
    float H = abs((Q.w - Q.y) / (6.0f * C + Epsilon) + Q.z);
    return float32_t3(H, C, Q.x);
}


float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);
    
    float32_t3 hcv = RGBtoHCV(output.rgb);
    // ある程度の明度以下ならば真っ暗を返す
    if (hcv.z <= gData.threshold) {
        return float32_t4(0.0f, 0.0, 0.0f, 1.0f);
    }

	return output;
}
