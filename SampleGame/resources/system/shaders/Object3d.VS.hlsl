#include "Object3d.hlsli"

VertexShaderOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // インデックスを抽出
    uint32_t v = gIndex[vertexID + (instanceID * 3)].vertex;
    uint32_t w = gIndex[vertexID + (instanceID * 3)].worldTransform;
    uint32_t isUI = gIndex[vertexID + (instanceID * 3)].isUI;

    VertexShaderOutput output;
    //output.position = gVertex[v].position;
    output.position = mul(mul(gVertex[v].position, gWorldTransform[w].wtf), (isUI ? gCommonData.vp2D : gCameraData.m));
    //output.position = mul(gVertex[v].position, gWorldTransform[w].wtf);
    output.worldPos = mul(gVertex[v].position, gWorldTransform[w].wtf).xyz;
    output.texcoord = gVertex[v].texcoord;
    output.normal = normalize(mul(gVertex[v].normal, transpose((float32_t3x3) gWorldTransform[w].inverse)));
    output.color = gVertex[v].color;
    
    output.id = vertexID + (instanceID * 3);

    return output;
}