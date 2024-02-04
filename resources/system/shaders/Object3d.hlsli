// 頂点データ
struct Vertex {
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
    float32_t4 color;
};
struct WorldTransform {
    float32_t4x4 wtf;
    float32_t4x4 inverse;
};

struct CameraData {
    float32_t4x4 m;  // !< ビュープロジェクション
    float32_t3 position;
};

// マテリアル
struct Material {
    float32_t4x4 uvTransform;
    int32_t enableLighting;
    float32_t shinines;
};

// 平行光源
struct DirectionalLight {
    float32_t4x4 viewProjection;  // !< ビュープロジェクション
    float32_t4 color;       // !< ライトの色
    float32_t3 direction;   // !< ライトの向き
    float intensity;        // !< 輝度
};
// 点光源
struct PointLight {
    float32_t4 color;       // !< ライトの色
    float32_t3 position;    // !< ライトのワールド座標
    float intensity;        // !< 輝度
    float radius;           // !< ライトの届く最大距離
    float decay;            // !< 減衰率
};

// インデックス情報の構造体
struct IndexInfo {
    uint32_t vertex;        // 実質頂点インデックスの役割
    uint32_t worldTransform; // ワールドトランスフォーム
    uint32_t material;      // マテリアル
    uint32_t tex2d;         // テクスチャ
    uint32_t isUI;          // UIで表示するかどうか
};
// 全体で共通のデータ
struct CommonData {
    float32_t4x4 vp2D;              // 2D用のビュープロジェクション
    uint32_t directionLightCount;   // 平行光源の数
    uint32_t pointLightCount;       // 点光源の数
};


// ** 両方用 ** //

// 各ストラクチャーバッファーのインデックス
StructuredBuffer<IndexInfo> gIndex : register(t0);
// 描画するViewProjection
ConstantBuffer<CameraData> gCameraData : register(b0);

// 全体描画で共通のデータ
ConstantBuffer<CommonData> gCommonData : register(b1);

// ** VS用 ** //

// 頂点データ
StructuredBuffer<Vertex> gVertex : register(t1);
// WorldTransform
StructuredBuffer<WorldTransform> gWorldTransform : register(t2);


// ** PS用 ** //

// マテリアル
StructuredBuffer<Material> gMaterial : register(t1);

// 平行光源
StructuredBuffer<DirectionalLight> gDirectionalLight : register(t2);
// 点光源
StructuredBuffer<PointLight> gPointLight : register(t3);

// テクスチャ
Texture2D<float32_t4> gTexture[] : register(t0, space1);
SamplerState gSampler : register(s0);

// シャドウマップのテクスチャ
Texture2D<float> gDirectionShadowMap[] : register(t0, space2);   // 平行光源
SamplerState gDirectionShadowMapSampler : register(s1);
TextureCube<float> gPointShadowMap[] : register(t0, space3);   // 点光源
SamplerState gPointShadowMapSampler : register(s2);


// 影の濃さ
const static float kShadowDensity = 0.2f;


// 頂点シェーダーの出力結果の構造体
struct VertexShaderOutput {
    float32_t4 position : SV_POSITION;
    float32_t3 worldPos : POSITION0;    // ワールド座標
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t4 color : COLOR0;
    uint32_t id : SV_InstanceID;        // VSで参照したIndexInfoの番号
};

