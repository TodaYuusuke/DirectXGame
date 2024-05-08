#include "PassThrough.hlsli"

float32_t3 GrayScale(float32_t3 color) {
    return dot(color, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

float32_t4 main(PSInput input) : SV_TARGET
{
	// 最終結果
    float32_t4 output;
	
    // レンダリングする座標
    float2 uv = input.texcoord;

	// サンプリング
    output = gTexture.Sample(gSampler, uv);
    // グレースケール化
    output.rgb = GrayScale(output.rgb);
    
    return output;
}