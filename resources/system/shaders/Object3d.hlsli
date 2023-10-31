// 頂点データ
struct Vertex {
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
    float32_t4 color;
};

// マテリアル
struct Material {
    float32_t4x4 uvTransform;
    int32_t enableLighting;
};

// 平行光源
struct DirectionalLight {
    float32_t4x4 viewProjection;
    float32_t4 color;       // !< ライトの色
    float32_t3 direction;   // !< ライトの向き
    float intensity;        // !< 輝度
};

// インデックス情報の構造体
struct IndexInfo {
    uint32_t vertex;        // 実質頂点インデックスの役割
    uint32_t cameraVP;      // カメラのビュープロジェクション行列
    uint32_t worldMatrix;   // ワールドトランスフォーム
    uint32_t material;      // マテリアル
    uint32_t tex2d;         // テクスチャ
};


// ** 両方用 ** //

// 各ストラクチャーバッファーのインデックス
StructuredBuffer<IndexInfo> gIndex : register(t0);
// 平行光源
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b0);


// ** VS用 ** //

// 頂点データ
StructuredBuffer<Vertex> gVertex : register(t1);
// カメラのViewProjection
StructuredBuffer<float32_t4x4> gCameraVP : register(t2);
// WorldTransform
StructuredBuffer<float32_t4x4> gWorldMatrix : register(t3);


// ** PS用 ** //

// マテリアル
StructuredBuffer<Material> gMaterial : register(t1);

// シャドウマップのテクスチャ
Texture2D<float> gShadowMap : register(t2);
SamplerState gShadowMapSampler : register(s0);
// テクスチャ
Texture2D<float32_t4> gTexture[] : register(t3);
SamplerState gSampler : register(s1);


// 頂点シェーダーの出力結果の構造体
struct VertexShaderOutput {
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t4 color : COLOR0;
    float32_t4 posSM : POSITION_SM;     // シャドウマッピング用の座標
    uint32_t id : SV_InstanceID;        // VSで参照したIndexInfoの番号
};