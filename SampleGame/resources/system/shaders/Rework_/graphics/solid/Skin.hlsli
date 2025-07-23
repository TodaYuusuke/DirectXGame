#include "Solid.hlsli"

StructuredBuffer<Well> Wells : register(t5);

struct Skinned {
    float32_t4 position;
    float32_t3 normal;
};

Skinned Skinning(Vertex v, uint32_t instanceIndex) {
    Skinned skinned;
    
    skinned.position = mul(v.position, Wells[v.jIndex.x + (instanceIndex * mMetadata.jSize)].skeletonSpaceMatrix) * v.weight.x;
    skinned.position += mul(v.position, Wells[v.jIndex.y + (instanceIndex * mMetadata.jSize)].skeletonSpaceMatrix) * v.weight.y;
    skinned.position += mul(v.position, Wells[v.jIndex.z + (instanceIndex * mMetadata.jSize)].skeletonSpaceMatrix) * v.weight.z;
    skinned.position += mul(v.position, Wells[v.jIndex.w + (instanceIndex * mMetadata.jSize)].skeletonSpaceMatrix) * v.weight.w;
    skinned.position.w = 1.0f;
    
    skinned.normal = mul(v.normal, (float32_t3x3) Wells[v.jIndex.x + (instanceIndex * mMetadata.jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.x;
    skinned.normal += mul(v.normal, (float32_t3x3) Wells[v.jIndex.y + (instanceIndex * mMetadata.jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.y;
    skinned.normal += mul(v.normal, (float32_t3x3) Wells[v.jIndex.z + (instanceIndex * mMetadata.jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.z;
    skinned.normal += mul(v.normal, (float32_t3x3) Wells[v.jIndex.w + (instanceIndex * mMetadata.jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.w;
    skinned.normal = normalize(skinned.normal);
    
    return skinned;
}