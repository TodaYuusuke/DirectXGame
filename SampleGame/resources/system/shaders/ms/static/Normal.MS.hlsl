#include "Common.hlsli"

[NumThreads(128, 1, 1)]      // スレッド数最大128
[OutputTopology("triangle")] // 出力形状は三角形
void main(
    in uint32_t gid  : SV_GroupID,
    in uint32_t gtid : SV_GroupThreadID,
    out vertices VSOutput outVerts[128],
    out indices uint32_t3     outIndices[128]
)
{
    // Meshlet取得
    Meshlet meshlet = mMeshlets[gid];

    // メッシュレット出力数を求める
    SetMeshOutputCounts(meshlet.VertCount, meshlet.PrimCount);

    if (gtid < meshlet.VertCount) {
        // 頂点インデックスの取得
        uint32_t vertexIndex = GetVertexIndex(meshlet, gtid);
        
        // 取得したインデックスから頂点座標を求める
        VSOutput vertex = mVertices[vertexIndex];
        
        // 出力する頂点のデータを求める
        outVerts[gtid] = vertex;    // ほとんどのデータはそのまま
        outVerts[gtid].pos = mul(vertex.pos, cCamera.viewProjection);   // ViewProjectionだけ掛ける
    }
    if (gtid < meshlet.PrimCount)
    {
        // プリミティブ情報のインデックス情報を取得
        uint32_t3 packedIndices = GetPrimitive(meshlet, gtid);
        
        // 出力するプリミティブを求める
        outIndices[gtid] = packedIndices;
        
        //outVerts[packedIndices.x];
        //outVerts[packedIndices.y];
        //outVerts[packedIndices.z];
    }

}