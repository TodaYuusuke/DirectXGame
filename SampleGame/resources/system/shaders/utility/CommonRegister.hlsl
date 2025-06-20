// ** Primitive & Mesh ** //
struct Vertex {
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
    float32_t4 color;
    float32_t4 weight;
    int32_t4 jIndex;    // JointIndex
    int32_t mIndex;     // MaterialIndex
};
struct WorldTransform {
    float32_t4x4 m;
    float32_t4x4 translate;
    float32_t4x4 rotate;
    float32_t4x4 scale;
    float32_t4x4 inverse;
};
struct Material {
    float32_t4x4 uvTransform;
    float32_t4 color;
    int32_t enableLighting;
    float32_t shinines;
    int32_t tIndex;     // TextureIndex
};


// ** Primitive ** //
struct IndexInfo {
    uint32_t vertex;
    uint32_t worldTransform;
    uint32_t material;
};

// ** Mesh ** //
struct Meshlet {
    uint32_t VertCount;
    uint32_t VertOffset;
    uint32_t PrimCount;
    uint32_t PrimOffset;
};
struct Well { // Used during skinning
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};
struct PayLoad { // For AS
    uint32_t groupID;
};
struct ModelMetaData {
    uint32_t instanceSize; // !< Instance Size
    uint32_t mSize; // !< Material Size
    uint32_t jSize; // !< Joint Size
    uint32_t transformOffset;
    uint32_t materialOffset;
};


// ** Rendering ** //
struct Camera {
    float32_t4x4 m;         // ViewProjection
    float32_t4x4 rotate;    // RotateMatrix for Billboard
    float32_t3 worldPos;
};

// ** Lights ** //
struct DirectionalLight {
    float32_t4x4 m;     // ViewProjection
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
struct PointLight {
    float32_t4 color;
    float32_t3 worldPos; // WorldPosition
    float intensity;
    float radius;
    float decay;
};


// ** Output ** //
struct VSOutput {
    float32_t4 position : SV_POSITION;
    float32_t3 worldPos : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal   : NORMAL0;
    float32_t4 color    : COLOR0;
    uint32_t id         : SV_InstanceID;
};
struct MSOutput {
    float32_t4 position : SV_POSITION0;
    float32_t3 worldPos : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal   : NORMAL0;
    float32_t4 color    : COLOR0;
    int32_t mIndex      : INDEX0;
};
struct ParticleColliderOutput {
    float32_t4 position : SV_POSITION0;
    float32_t3 worldPos : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal   : NORMAL0;
    float32_t4 color    : COLOR0;
    int32_t mIndex      : INDEX0;
    uint32_t id         : INDEX1;
};
