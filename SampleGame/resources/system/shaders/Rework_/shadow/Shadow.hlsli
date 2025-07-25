#include "../Structs.hlsli"

struct ViewProjection {
    float32_t4x4 m;
};

// Common
ConstantBuffer<ViewProjection> cViewProjection : register(b0);  // light view

// ModelData
ConstantBuffer<ModelMetadata> mMetadata : register(b1);
StructuredBuffer<Meshlet> mMeshlets : register(t0);
StructuredBuffer<Vertex> mVertices : register(t1);
ByteAddressBuffer mUniqueVertexIndices : register(t2);
StructuredBuffer<uint32_t> mPrimitiveIndices : register(t3);

// InstanceBuffers
StructuredBuffer<WorldTransform> iWorldTransform : register(t4);