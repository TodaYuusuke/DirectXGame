#include "Object3d.hlsli"

VertexShaderOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // インデックスを抽出
    uint32_t v = gIndex[vertexID + (instanceID * 3)].vertex;
    uint32_t c = gIndex[vertexID + (instanceID * 3)].cameraVP;
    uint32_t w = gIndex[vertexID + (instanceID * 3)].worldMatrix;

    VertexShaderOutput output;
    output.position = mul(mul(gVertex[v].position, gWorldMatrix[w]), gCameraVP[c]);
    output.WorldPos = mul(gVertex[v].position, gWorldMatrix[w]).xyz;
    output.texcoord = gVertex[v].texcoord;
    output.normal = normalize(mul(gVertex[v].normal, (float32_t3x3)gWorldMatrix[w]));
    output.color = gVertex[v].color;
    
    // 平行光源のviewProjectionに対してシャドウマップ用の座標を計算する
    float4 Pos = mul(mul(gVertex[v].position, gWorldMatrix[w]), gLightViewProjection[0]);
    Pos.xyz = Pos.xyz / Pos.w;
    output.posSM.x = (1.0f + Pos.x) / 2.0f;
    output.posSM.y = (1.0f - Pos.y) / 2.0f;
    output.posSM.z = Pos.z;
    
    output.id = vertexID + (instanceID * 3);

    return output;
}