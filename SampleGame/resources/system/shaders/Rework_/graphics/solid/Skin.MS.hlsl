#include "Solid.hlsli"

StructuredBuffer<Well> Wells : register(t5);

[NumThreads(128, 1, 1)]      // スレッド数最大128
[OutputTopology("triangle")] // 出力形状は三角形
void main(
    in uint32_t gid  : SV_GroupID,
    in uint32_t gtid : SV_GroupThreadID,
    in payload PayLoad meshPayload,
    out vertices MSOutput outVerts[128],
    out indices uint32_t3     outIndices[128]
) {
    // Meshlet取得
    Meshlet meshlet = mMeshlets[meshPayload.groupID];

    // メッシュレット出力数を求める
    SetMeshOutputCounts(meshlet.VertCount, meshlet.PrimCount);

    if (gtid < meshlet.VertCount) {
        // 頂点インデックスの取得
        uint32_t vertexIndex = GetVertexIndex(meshlet, gtid, mUniqueVertexIndices);
        
        // 取得したインデックスから頂点座標を求める
        Vertex vertex = mVertices[vertexIndex];
        // 出力する頂点のデータを求める
        Skinned skinned = Skinning(vertex, gid, mMetadata.jSize, Wells);

        outVerts[gtid].position = mul(mul(skinned.position, iWorldTransform[gid].m), cCamera.m);
        outVerts[gtid].worldPos = mul(skinned.position, iWorldTransform[gid].m).xyz;
        outVerts[gtid].texcoord = vertex.texcoord;
        outVerts[gtid].normal = normalize(mul(skinned.normal, transpose((float32_t3x3) iWorldTransform[gid].inverse)));
        outVerts[gtid].color = vertex.color;
        outVerts[gtid].mIndex = vertex.mIndex + (gid * mMetadata.mSize); // マテリアルのインデックスをインスタンス番号分ずらす
    }
    if (gtid < meshlet.PrimCount) {
        // プリミティブ情報のインデックス情報を取得し、出力するプリミティブを求める
        outIndices[gtid] = GetPrimitive(meshlet, gtid, mPrimitiveIndices);
    }
}