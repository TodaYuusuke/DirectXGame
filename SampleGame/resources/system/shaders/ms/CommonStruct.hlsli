// ** Standard Structs ** //
struct WorldTransform
{
    float32_t4x4 m;
    float32_t4x4 translate; // !< translate only Matrix
    float32_t4x4 rotate; // !< rotate only Matrix
    float32_t4x4 scale; // !< scale only Matrix
    float32_t4x4 inverse;
};


// ** ModelData ** //
struct Meshlet
{
    uint32_t VertCount;
    uint32_t VertOffset;
    uint32_t PrimCount;
    uint32_t PrimOffset;
};
struct Vertex
{
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
    float32_t4 color;
    float32_t4 weight;
    int32_t4 jIndex; // !< JointIndex
    int32_t mIndex; // !< MaterialIndex
};

// ** InstanceData ** //
struct InstanceData
{
    WorldTransform wtf;
    //int32_t enableLighting; // !< Flag(bool)
};
struct Well
{ // !< Used during skinning
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

// **  ** //
struct PayLoad
{
    uint32_t groupID;
};
struct ModelCommon
{
    uint32_t instanceSize; // !< Instance Size
    uint32_t mSize; // !< Material Size
    uint32_t jSize; // !< Joint Size
};


// ** CommonData ** //
struct CommonData
{
    float32_t4x4 vp2D;
    uint32_t directionLightCount;
    uint32_t pointLightCount;
};

struct Camera
{
    float32_t4x4 viewProjection; // !< ViewProjection
    float32_t4x4 rotate; // !< RotateMatrix for Billboard
    float32_t3 position; // !< WorldPosition
};
struct Material
{
    float32_t4x4 uvTransform;
    float32_t4 color;
    int32_t enableLighting; // !< Not Using(But it's being used in Object3d.PS.hlsl, And will be deleted when Primitive is reworked)
    float32_t shinines;
    int32_t tIndex; // !< TextureIndex
};

// ** Lights ** //
struct DirectionalLight
{
    float32_t4x4 viewProjection;
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
struct PointLight
{
    float32_t4 color;
    float32_t3 position; // !< WorldPosition
    float intensity;
    float radius;
    float decay;
};


// ** Output ** //
struct VSOutput
{
    float32_t4 pos : SV_POSITION0;
    float32_t3 worldPos : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t4 color : COLOR0;
    int32_t mIndex : INDEX0;
};
