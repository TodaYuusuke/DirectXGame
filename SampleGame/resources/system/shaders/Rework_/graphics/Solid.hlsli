#include "../Structs.hlsl"

// Common
ConstantBuffer<Camera> cCamera : register(b0);
Texture2D<float32_t4> cTexture[] : register(t0, space1);
SamplerState cTexSmp : register(s0);

// ModelData
ConstantBuffer<ModelMetadata> mMetadata : register(b1);
StructuredBuffer<Meshlet> mMeshlets : register(t0);
StructuredBuffer<Vertex> mVertices : register(t1);
ByteAddressBuffer mUniqueVertexIndices : register(t2);
StructuredBuffer<uint32_t> mPrimitiveIndices : register(t3);

// InstanceBuffers
StructuredBuffer<WorldTransform> iWorldTransform : register(t4);
StructuredBuffer<Material> iMaterials : register(t5);
