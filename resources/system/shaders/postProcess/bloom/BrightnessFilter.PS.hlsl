
struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

float32_t3 GrayScale(float32_t3 color) {
    return dot(color, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);

	output.rgb = GrayScale(output.rgb);
    float brightness = output.r + output.g + output.b;
    brightness = brightness / 3.0f;
    
    // ある程度の輝度以下ならば真っ暗を返す
    if (brightness <= 0.96f) {
        return float32_t4(0.0f, 0.0, 0.0f, 1.0f);
    }

	return output;
}
