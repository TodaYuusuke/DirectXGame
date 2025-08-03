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
struct ModelMetadata {
    uint32_t instanceSize; // !< Instance Size
    uint32_t mSize; // !< Material Size
    uint32_t jSize; // !< Joint Size
};


// ** Rendering ** //
struct Camera {
    float32_t4x4 m;         // ViewProjection
    float32_t4x4 vp2D;      // ViewProjection for Sprite 
    float32_t4x4 rotate;    // RotateMatrix for Billboard
    float32_t3 worldPos;
};

// ** Lights ** //
struct DirectionalLight {
    float32_t4x4 m;     // ViewProjection
    float32_t4 color;
    float32_t3 direction;
    float intensity;
    float shadowIntensity;
    float bias;
};
struct PointLight {
    float32_t4 color;
    float32_t3 worldPos; // WorldPosition
    float intensity;
    float radius;
    float decay;
};
struct LightMetadata {
    int32_t pointLightCount; // Point Light Count
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
struct VSOutputUnlit {  // No lighting
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t4 color : COLOR0;
    uint32_t id         : SV_InstanceID;
};
struct PassThroughOutput{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct MSOutput {
    float32_t4 position : SV_POSITION0;
    float32_t3 worldPos : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal   : NORMAL0;
    float32_t4 color    : COLOR0;
    int32_t mIndex      : INDEX0;
};
struct MSShadowOutput {
    float32_t4 position : SV_POSITION;
};
struct GBufferOutput {
    float32_t4 baseColor : SV_Target0;
    float32_t4 normal : SV_Target1;     // xyz = normal(0 ~ 1), w = enableLighting Flag
    float32_t4 pbr : SV_Target2;
    float32_t4 worldPosition : SV_Target3;
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


float32_t3 EncodeNormal(float32_t3 n) {
    return n * 0.5f + 0.5f;
}
float32_t3 DecodeNormal(float32_t3 n) {
    return n * 2.0f - 1.0f;
}


///
///  Mesh Shader Functions
///

uint32_t3 UnpackPrimitive(uint primitive) {
    // Read the index of the primitive every 10 bits
    return uint32_t3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}
uint32_t3 GetPrimitive(Meshlet m, uint index, StructuredBuffer<uint32_t> primitiveIndices) {
    // Get surface information
    return UnpackPrimitive(primitiveIndices[m.PrimOffset + index]);
}
uint32_t GetVertexIndex(Meshlet m, uint localIndex, ByteAddressBuffer uniqueVertexIndices) {
    // Find the index of the vertex
    localIndex = m.VertOffset + localIndex;
    // Read 4 bytes at a time
    return uniqueVertexIndices.Load(localIndex * 4);
}


///
/// Skin Functions
///

struct Skinned {
    float32_t4 position;
    float32_t3 normal;
};
Skinned Skinning(Vertex v, uint32_t instanceIndex, uint32_t jSize, StructuredBuffer<Well> wells) {
    Skinned skinned;
    
    skinned.position = mul(v.position, wells[v.jIndex.x + (instanceIndex * jSize)].skeletonSpaceMatrix) * v.weight.x;
    skinned.position += mul(v.position, wells[v.jIndex.y + (instanceIndex * jSize)].skeletonSpaceMatrix) * v.weight.y;
    skinned.position += mul(v.position, wells[v.jIndex.z + (instanceIndex * jSize)].skeletonSpaceMatrix) * v.weight.z;
    skinned.position += mul(v.position, wells[v.jIndex.w + (instanceIndex * jSize)].skeletonSpaceMatrix) * v.weight.w;
    skinned.position.w = 1.0f;
    
    skinned.normal = mul(v.normal, (float32_t3x3) wells[v.jIndex.x + (instanceIndex * jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.x;
    skinned.normal += mul(v.normal, (float32_t3x3) wells[v.jIndex.y + (instanceIndex * jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.y;
    skinned.normal += mul(v.normal, (float32_t3x3) wells[v.jIndex.z + (instanceIndex * jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.z;
    skinned.normal += mul(v.normal, (float32_t3x3) wells[v.jIndex.w + (instanceIndex * jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.w;
    skinned.normal = normalize(skinned.normal);
    
    return skinned;
}