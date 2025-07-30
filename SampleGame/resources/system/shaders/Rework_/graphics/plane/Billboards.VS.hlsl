#include "BillboardFunction.hlsli"

VSOutputUnlit main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // ソートされた順番に描画する
    int32_t index = vSortIndex[instanceID];
    
    // 頂点を抽出
    Vertex v = GetVertexIndex(vertexID, index);
    // ワールドトランスフォームを抽出q
    WorldTransform wtf = vWorldTransform[index];
    // ビルボードの種類を求める
    int32_t type = vBillboardType[index];
    
    // 出力
    VSOutputUnlit output;
    
    switch (type) {
        case 1:     // Surface
            output.position = Surface(v, wtf);
            break;
        case 2:     // Billboard2D
            output.position = Billboard2D(v, wtf);
            break;
        case 3:     // HorizontalBillboard
            output.position = HorizontalBillboard(v, wtf, index);
            break;
        case 4:     // VerticalBillboard
            output.position = VerticalBillboard(v, wtf, index);
            break;
        case 5:     // StretchedBillboard
            output.position = StretchedBillboard(v, wtf, vVelocity[index], index);
            break;
        default:    // Sprite or エラー
            output.position = v.position;
            break;
    }
    
    output.texcoord = v.texcoord;
    output.color = v.color;
    output.id = index;

    return output;
}