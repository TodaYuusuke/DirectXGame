#include "../CommonStruct.hlsli"
#include "../CommonConstant.hlsli"

// ** Define common binding information here  ** //

// ModelData
StructuredBuffer<Meshlet> mMeshlets : register(t0);
StructuredBuffer<VSOutput> mVertices : register(t1);
ByteAddressBuffer mUniqueVertexIndices : register(t2);
StructuredBuffer<uint32_t> mPrimitiveIndices : register(t3);

// CommonBuffers
ConstantBuffer<CommonData> cCommonData : register(b0);
ConstantBuffer<Camera> cCamera : register(b1);
StructuredBuffer<Material> cMaterials : register(t4);
StructuredBuffer<DirectionalLight> cDLights : register(t5);
StructuredBuffer<PointLight> cPLights : register(t6);

// Textures
Texture2D<float32_t4> tTexture[kMaxSize_Texture] : register(t7);
Texture2D<float> tDLightSM[kMaxSize_DirSM] : register(t507);
TextureCube<float> tPLightSM[kMaxSize_PointSM] : register(t508);

SamplerState sTexSmp : register(s0);
SamplerState sDLightSMSmp : register(s1);
SamplerState sPLightSMSmp : register(s2);


// ** Functions ** //

uint32_t3 UnpackPrimitive(uint primitive)
{
    // Read the index of the primitive every 10 bits
    return uint32_t3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}

uint32_t3 GetPrimitive(Meshlet m, uint index)
{
    // Get surface information
    return UnpackPrimitive(mPrimitiveIndices[m.PrimOffset + index]);
}

uint32_t GetVertexIndex(Meshlet m, uint localIndex)
{
    // Find the index of the vertex
    localIndex = m.VertOffset + localIndex;
    // Read 4 bytes at a time
    return mUniqueVertexIndices.Load(localIndex * 4);
}