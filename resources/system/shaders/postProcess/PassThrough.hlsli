// 頂点データを必要としない（フルスクリーンレンダリング前提の）PixelShader用のVertexShader

// PixelShaderが受け取る構造体
struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};