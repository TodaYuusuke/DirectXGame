#include "PassThrough.hlsli"

// パラメータ
struct Parameter
{
    int time; // !< 時間
    int rWidth; // !< 解像度（横幅）
    int rHeight; // !< 解像度（縦幅） 
};

// パラメータのバインド
ConstantBuffer<Parameter> gPara : register(b0);
// ポストプロセスをかけるテクスチャ（ちゃんとTexture2D二枚で渡せるようにいつか修正する）
// 0 ... 深度マップ
// 1 ... レンダリング結果
Texture2D<float32_t4> gTexture[] : register(t0);
//Texture2D<float32_t4> gDepth : register(t1);
// サンプラー
SamplerState gSampler : register(s0);


float32_t4 main(PSInput input) : SV_TARGET
{
	// 最終結果
    float32_t4 output;
	
    // レンダリングする座標
    float2 uv = input.texcoord;

	// サンプリング
    output = gTexture[1].Sample(gSampler, uv);

    return output;
}