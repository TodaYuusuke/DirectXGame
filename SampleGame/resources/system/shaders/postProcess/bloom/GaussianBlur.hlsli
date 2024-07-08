struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const float32_t PI = 3.14159265f;

static const float32_t kSpI = 2.0f; // inline
static const float32_t kSpO = 4.0f; // outline

//static const float32_t2 kIndex3x3[5][5] = {
//    { { -kSpO, -kSpO }, { -kSpI, -kSpO }, { +0.0f, -kSpO }, { +kSpI, -kSpO }, { +kSpO, -kSpO } },
//    { { -kSpO, -kSpI }, { -kSpI, -kSpI }, { +0.0f, -kSpI }, { +kSpI, -kSpI }, { +kSpO, -kSpI } },
//    { { -kSpO, +0.0f }, { -kSpI, +0.0f }, { +0.0f, +0.0f }, { +kSpI, +0.0f }, { +kSpO, +0.0f } },
//    { { -kSpO, +kSpI }, { -kSpI, +kSpI }, { +0.0f, +kSpI }, { +kSpI, +kSpI }, { +kSpO, +kSpI } },
//    { { -kSpO, +kSpO }, { -kSpI, +kSpO }, { +0.0f, +kSpO }, { +kSpI, +kSpO }, { +kSpO, +kSpO } },
//};


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

