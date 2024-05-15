#include "Struct.hlsli"

struct SkinningVertex {
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
    float32_t4 color;
    float32_t4 weight;
    int32_t4 index;
};

struct Skinned {
    float32_t4 position;
    float32_t3 normal;
};
struct Well {
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};


// ** for VS and PS ** //
StructuredBuffer<IndexInfo> gIndex : register(t0);
ConstantBuffer<CameraData> gCameraData : register(b0);

ConstantBuffer<CommonData> gCommonData : register(b1);

// ** for VS ** //
StructuredBuffer<SkinningVertex> gVertex : register(t1);
StructuredBuffer<Well> gWell : register(t2);
StructuredBuffer<WorldTransform> gWorldTransform : register(t3);



Skinned Skinning(SkinningVertex v)
{
    Skinned skinned;
    
    skinned.position = mul(v.position, gWell[v.index.x].skeletonSpaceMatrix) * v.weight.x;
    skinned.position += mul(v.position, gWell[v.index.y].skeletonSpaceMatrix) * v.weight.y;
    skinned.position += mul(v.position, gWell[v.index.z].skeletonSpaceMatrix) * v.weight.z;
    skinned.position += mul(v.position, gWell[v.index.w].skeletonSpaceMatrix) * v.weight.w;
    skinned.position.w = 1.0f;
    
    skinned.normal = mul(v.normal, (float32_t3x3) gWell[v.index.x].skeletonSpaceInverseTransposeMatrix) * v.weight.x;
    skinned.normal = mul(v.normal, (float32_t3x3) gWell[v.index.y].skeletonSpaceInverseTransposeMatrix) * v.weight.y;
    skinned.normal = mul(v.normal, (float32_t3x3) gWell[v.index.z].skeletonSpaceInverseTransposeMatrix) * v.weight.z;
    skinned.normal = mul(v.normal, (float32_t3x3) gWell[v.index.w].skeletonSpaceInverseTransposeMatrix) * v.weight.w;
    skinned.normal = normalize(skinned.normal);
    
    return skinned;
}


VertexShaderOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // インデックスを抽出
    uint32_t v = gIndex[vertexID + (instanceID * 3)].vertex;
    uint32_t w = gIndex[vertexID + (instanceID * 3)].worldTransform;
    uint32_t isUI = gIndex[vertexID + (instanceID * 3)].isUI;

    VertexShaderOutput output;
    Skinned skinned = Skinning(gVertex[v]);
    output.position = mul(mul(skinned.position, gWorldTransform[w].wtf), (isUI ? gCommonData.vp2D : gCameraData.m));
    output.worldPos = mul(skinned.position, gWorldTransform[w].wtf).xyz;
    output.texcoord = gVertex[v].texcoord;
    output.normal = normalize(mul(skinned.normal, transpose((float32_t3x3) gWorldTransform[w].inverse)));
    output.color = gVertex[v].color;
    
    output.id = vertexID + (instanceID * 3);

    return output;
}