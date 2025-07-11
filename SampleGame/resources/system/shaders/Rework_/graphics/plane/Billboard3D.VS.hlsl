#include "Plane.hlsli"

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

VSOutputUnlit main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // 頂点を抽出
    Vertex v = GetSpriteVertexIndex(vertexID, instanceID);
    // ワールドトランスフォームを抽出
    WorldTransform wtf = vWorldTransform[instanceID];
    
    // 出力
    VSOutputUnlit output;
    
    // ビルボードの座標
    float32_t3 worldPos = mul(v.position, wtf.m).xyz;
    // billboardの向いている方向
    float32_t3 from = normalize(float3(0.0f, 0.0f, -1.0f));
    // ビルボードからカメラ方向のベクトル
    float32_t3 to = normalize(vCamera.worldPos - worldPos);
    // 回転行列を求める
    float32_t4x4 m = DirectionToDirection(from, to);
    
    output.position = mul(mul(mul(v.position, m), wtf.m), vCamera.m);
    output.texcoord = v.texcoord;
    output.color = v.color;
    output.id = instanceID;

    return output;
}