// WorldTransform
struct WorldMatrix {
    float32_t4x4 World;
};
ConstantBuffer<WorldMatrix> gWorldMatrix : register(b0);

// 平行光源
struct DirectionalLight {
    float32_t4x4 viewProjection;
    float32_t4 color;       // !< ライトの色
    float32_t3 direction;   // !< ライトの向き
    float intensity;        // !< 輝度
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);


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