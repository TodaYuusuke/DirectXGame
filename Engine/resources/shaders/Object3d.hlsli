// マテリアル
struct Material {
    float32_t4x4 uvTransform;
    int32_t enableLighting;
};
ConstantBuffer<Material> gMaterial : register(b0);

// カメラのViewProjection
struct TransformationMatrix {
    float32_t4x4 WVP;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
// WorldTransform
struct WorldMatrix {
    float32_t4x4 World;
};
ConstantBuffer<WorldMatrix> gWorldMatrix : register(b1);

// テクスチャ
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// シャドウマップのテクスチャサンプラー
Texture2D<float> gShadowMap : register(t1);
SamplerState gShadowMapSampler : register(s1);

// 平行光源
struct DirectionalLight {
    float32_t4x4 viewProjection;
    float32_t4 color;       // !< ライトの色
    float32_t3 direction;   // !< ライトの向き
    float intensity;        // !< 輝度
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);


struct VertexShaderOutput {
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t4 color : COLOR0;
    float32_t4 posSM : POSITION_SM;
};

struct VertexShaderInput {
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t4 color : COLOR0;
};