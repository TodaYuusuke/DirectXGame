#include "Plane.hlsli"

VSOutputUnlit main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // ソートされた順番に描画する
    int32_t index = vSortIndex[instanceID];
    
    // 頂点を抽出
    Vertex v = GetVertexIndex(vertexID, index);
    // ワールドトランスフォームを抽出
    WorldTransform wtf = vWorldTransform[index];
    
    // 出力
    VSOutputUnlit output;
    
    float32_t4x4 mat = mul(mul(wtf.scale, mul(wtf.rotate, vCamera.rotate)), wtf.translate);
    //float32_t4x4 mat = mul(mul(wtf.scale, vCamera.rotate), wtf.translate);
    output.position = mul(mul(v.position, mat), vCamera.m);
    output.texcoord = v.texcoord;
    output.color = v.color;
    output.id = index;

    return output;
}