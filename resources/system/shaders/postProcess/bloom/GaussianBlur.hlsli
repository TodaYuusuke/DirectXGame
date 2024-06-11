struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const float32_t PI = 3.14159265f;

static const float32_t kSpI = 2.0f;
static const float32_t kSpO = 4.0f;

static const uint32_t kIndexSize = 31;
static const float32_t kIndexMultiply = 1.0f;
static const float32_t kStandardDeviation = 5.0f;

float GetIndex(float index) {
    return (index - (kIndexSize / 2.0f)) * kIndexMultiply;
}

float Gauss(float x, float y, float sigma) {
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

