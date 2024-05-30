struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const float32_t PI = 3.14159265f;

static const float32_t kSpI = 2.0f; // 
static const float32_t kSpO = 4.0f; // 

static const float32_t2 kIndex3x3[5][5] = {
    { { -kSpO, -kSpO }, { -kSpI, -kSpO }, { +0.0f, -kSpO }, { +kSpI, -kSpO }, { +kSpO, -kSpO } },
    { { -kSpO, -kSpI }, { -kSpI, -kSpI }, { +0.0f, -kSpI }, { +kSpI, -kSpI }, { +kSpO, -kSpI } },
    { { -kSpO, +0.0f }, { -kSpI, +0.0f }, { +0.0f, +0.0f }, { +kSpI, +0.0f }, { +kSpO, +0.0f } },
    { { -kSpO, +kSpI }, { -kSpI, +kSpI }, { +0.0f, +kSpI }, { +kSpI, +kSpI }, { +kSpO, +kSpI } },
    { { -kSpO, +kSpO }, { -kSpI, +kSpO }, { +0.0f, +kSpO }, { +kSpI, +kSpO }, { +kSpO, +kSpO } },
};
//static const float32_t kKernel[3][3] = {
//    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
//    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
//    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f }
//};

float Gauss(float x, float y, float sigma) {
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

float32_t4 main(PSInput input) : SV_TARGET
{
    float32_t4 output;

    float32_t weight = 0.0f;
    float32_t kernel3x3[5][5];
    // BoxFilter
    uint32_t width, height; // 1. uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
    output.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.a = 1.0f;
    for (int32_t x = 0; x < 5; x++) {
        for (int32_t y = 0; y < 5; y++) {
            kernel3x3[x][y] = Gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, 500.0f);
            weight += kernel3x3[x][y];
            
            // 現在のtexcoordを算出
            float32_t2 uv = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            // 色に1/9掛けて足す
            float32_t3 fetchColor = gTexture.Sample(gSampler, uv).rgb;
            output.rgb += fetchColor * kernel3x3[x][y];
        }
    }
    
    // 畳み込み後の正規化する
    output.rgb *= rcp(weight);
    
    return output;
}
