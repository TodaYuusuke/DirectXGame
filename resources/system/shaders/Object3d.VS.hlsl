#include "Object3d.hlsli"

VertexShaderOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // インデックスを抽出
    uint32_t v = gIndex[vertexID + (instanceID * 3)].vertex;
    uint32_t w = gIndex[vertexID + (instanceID * 3)].worldMatrix;
    uint32_t isUI = gIndex[vertexID + (instanceID * 3)].isUI;

    VertexShaderOutput output;
    output.position = mul(mul(gVertex[v].position, gWorldMatrix[w]), (isUI ? gCommonData.vp2D : gViewProjection.m));
    output.worldPos = mul(gVertex[v].position, gWorldMatrix[w]).xyz;
    output.texcoord = gVertex[v].texcoord;
    output.normal = normalize(mul(gVertex[v].normal, (float32_t3x3)gWorldMatrix[w]));
    output.color = gVertex[v].color;
    
    output.id = vertexID + (instanceID * 3);

    return output;
}