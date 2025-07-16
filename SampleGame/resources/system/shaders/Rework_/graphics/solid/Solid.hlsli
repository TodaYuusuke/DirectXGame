#include "../../Structs.hlsli"

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

uint32_t3 UnpackPrimitive(uint primitive) {
    // Read the index of the primitive every 10 bits
    return uint32_t3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}

uint32_t3 GetPrimitive(Meshlet m, uint index) {
    // Get surface information
    return UnpackPrimitive(mPrimitiveIndices[m.PrimOffset + index]);
}

uint32_t GetVertexIndex(Meshlet m, uint localIndex) {
    // Find the index of the vertex
    localIndex = m.VertOffset + localIndex;
    // Read 4 bytes at a time
    return mUniqueVertexIndices.Load(localIndex * 4);
}