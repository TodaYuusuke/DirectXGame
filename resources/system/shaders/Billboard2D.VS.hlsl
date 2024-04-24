#include "Object3d.hlsli"

VertexShaderOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // インデックスを抽出
    uint32_t v = gIndex[vertexID + (instanceID * 3)].vertex;
    uint32_t w = gIndex[vertexID + (instanceID * 3)].worldTransform;

    VertexShaderOutput output;
    // 掛けるためのtransformを生成
    //float32_t4x4 mat = mul(mul(gWorldTransform[w].scale, view), gWorldTransform[w].translate);
    // 最後に掛ける
    //output.position = mul(gVertex[v].position, mul(gCameraData.inverse, gWorldTransform[w].wtf));
    
    float32_t4x4 mat = mul(mul(gWorldTransform[w].scale, gCameraData.rotate), gWorldTransform[w].translate);
    output.position = mul(mul(gVertex[v].position, mat), gCameraData.m);
    
    output.worldPos = mul(gVertex[v].position, gWorldTransform[w].wtf).xyz;
    output.texcoord = gVertex[v].texcoord;
    output.normal = normalize(mul(gVertex[v].normal, (float32_t3x3)gWorldTransform[w].inverse));
    output.color = gVertex[v].color;
    
    output.id = vertexID + (instanceID * 3);

    return output;
}