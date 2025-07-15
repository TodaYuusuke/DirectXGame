#include "Plane.hlsli"

VSOutputUnlit main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // 頂点を抽出
    Vertex v = GetVertexIndex(vertexID, instanceID);
    // ワールドトランスフォームを抽出
    WorldTransform wtf = vWorldTransform[instanceID];

    // 出力
    VSOutputUnlit output;
    
    output.position = mul(mul(v.position, wtf.m), vCamera.vp2D);
    output.texcoord = v.texcoord;
    output.color = v.color;
    output.id = instanceID;

    return output;
}