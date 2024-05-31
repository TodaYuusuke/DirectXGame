struct Vertex {
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
    float32_t4 color;
    float32_t4 weight;
    int32_t4 jIndex; // !< JointIndex
    int32_t mIndex; // !< MaterialIndex
};
struct WorldTransform {
    float32_t4x4 wtf;
    float32_t4x4 translate;
    float32_t4x4 rotate;
    float32_t4x4 scale;
    float32_t4x4 inverse;
};

struct CameraData {
    float32_t4x4 m;  // !< ViewProjection
    float32_t4x4 rotate; // !< RotateMatrix for Billboard
    float32_t3 position;
};

struct Material {
    float32_t4x4 uvTransform;
    float32_t4 color;
    int32_t enableLighting;
    float32_t shinines;
    int32_t tIndex; // !< TextureIndex
};

struct DirectionalLight {
    float32_t4x4 viewProjection;
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
struct PointLight {
    float32_t4 color;
    float32_t3 position;    // !< WorldPosition
    float intensity;
    float radius;
    float decay;
};

struct IndexInfo {
    uint32_t vertex;
    uint32_t worldTransform;
    uint32_t material;
    uint32_t isUI;
};
struct CommonData {
    float32_t4x4 vp2D;
    uint32_t directionLightCount;
    uint32_t pointLightCount;
};

struct VertexShaderOutput {
    float32_t4 position : SV_POSITION;
    float32_t3 worldPos : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t4 color : COLOR0;
    uint32_t id : SV_InstanceID;
};

