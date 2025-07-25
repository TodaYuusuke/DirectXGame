#include "Plane.hlsli"

VSOutputUnlit main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // ソートされた順番に描画する
    int32_t index = vSortIndex[instanceID];
    
    // 頂点を抽出
    Vertex v = GetVertexIndex(vertexID, index);
    // ワールドトランスフォームを抽出q
    WorldTransform wtf = vWorldTransform[index];

    // 出力
    VSOutputUnlit output;
    
    output.position = mul(mul(v.position, wtf.m), vCamera.vp2D);
    output.texcoord = v.texcoord;
    output.color = v.color;
    output.id = index;

    return output;
}