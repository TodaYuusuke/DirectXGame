#include "Plane.hlsli"

static const uint32_t kNumInstance = 6;
static const uint32_t kVertexID[kNumInstance] = {
    0, 1, 2, 0, 2, 3
};

VSOutputUnlit main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // 頂点を抽出
    Vertex v = vVertex[(instanceID * 4) + kVertexID[vertexID]];
    // ワールドトランスフォームを取得
    WorldTransform wtf = vWorldTransform[instanceID];

    // 出力
    VSOutputUnlit output;
    
    output.position = mul(mul(v.position, wtf.m), vCamera.vp2D);
    output.texcoord = v.texcoord;
    output.color = v.color;
    output.id = instanceID;

    return output;
}