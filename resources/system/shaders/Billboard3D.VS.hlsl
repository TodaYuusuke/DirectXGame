#include "Object3d.hlsli"

float4x4 DirectionToDirection(float3 from, float3 to) {
    float c = dot(from, to);
    float s = length(cross(from, to));
    float3 n = normalize(cross(from, to));
    n = c == -1.0f ? float3(from.y, -from.x, 0.0f) : n;

    float32_t4x4 result;
    result[0][0] = (n.x * n.x) * (1.0f - c) + c;
    result[0][1] = (n.x * n.y) * (1.0f - c) + n.z * s;
    result[0][2] = (n.x * n.z) * (1.0f - c) - n.y * s;
    result[0][3] = 0.0f;

    result[1][0] = (n.y * n.x) * (1.0f - c) - n.z * s;
    result[1][1] = (n.y * n.y) * (1.0f - c) + c;
    result[1][2] = (n.y * n.z) * (1.0f - c) + n.x * s;
    result[1][3] = 0.0f;

    result[2][0] = (n.z * n.x) * (1.0f - c) + n.y * s;
    result[2][1] = (n.z * n.y) * (1.0f - c) - n.x * s;
    result[2][2] = (n.z * n.z) * (1.0f - c) + c;
    result[2][3] = 0.0f;

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;

    return result;
}

VertexShaderOutput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // インデックスを抽出
    uint32_t v = gIndex[vertexID + (instanceID * 3)].vertex;
    uint32_t w = gIndex[vertexID + (instanceID * 3)].worldTransform;

    VertexShaderOutput output;
    
    output.worldPos = mul(gVertex[v].position, gWorldTransform[w].wtf).xyz;
    
    // billboardの向いている方向
    float3 from = normalize(float3(0.0f, 0.0f, -1.0f));
    // ビルボードからカメラ方向のベクトル
    float3 to = normalize(gCameraData.position - output.worldPos);
    float4x4 m = DirectionToDirection(from, to);
    output.position = mul(mul(mul(gVertex[v].position, m), gWorldTransform[w].wtf), gCameraData.m);
    
    //float4x4 m = DirectionToDirection(output.worldPos, gCameraData.position);
    //output.position = mul(mul(gVertex[v].position, gWorldTransform[w].wtf), gCameraData.m);
    
    output.texcoord = gVertex[v].texcoord;
    output.normal = normalize(mul(gVertex[v].normal, (float32_t3x3)gWorldTransform[w].inverse));
    output.color = gVertex[v].color;
    
    output.id = vertexID + (instanceID * 3);

    return output;
}