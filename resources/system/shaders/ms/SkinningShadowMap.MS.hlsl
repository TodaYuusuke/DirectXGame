#include "MSStruct.hlsli"

struct ShadowMapOutput
{
    float4 position : SV_POSITION;
};
struct ViewProjection
{
    float32_t4x4 m;
};

ConstantBuffer<ViewProjection> gViewProjection : register(b1);

StructuredBuffer<Well> Wells : register(t5);

float32_t4 Skinning(Vertex v) {
    float32_t4 result;
    
    result = mul(v.position, Wells[v.jIndex.x].skeletonSpaceMatrix) * v.weight.x;
    result += mul(v.position, Wells[v.jIndex.y].skeletonSpaceMatrix) * v.weight.y;
    result += mul(v.position, Wells[v.jIndex.z].skeletonSpaceMatrix) * v.weight.z;
    result += mul(v.position, Wells[v.jIndex.w].skeletonSpaceMatrix) * v.weight.w;
    result.w = 1.0f;
    
    return result;
}

[NumThreads(128, 1, 1)]      // スレッド数最大128
[OutputTopology("triangle")] // 出力形状は三角形
void main(
    in uint32_t gid  : SV_GroupID,
    in uint32_t gtid : SV_GroupThreadID,
    in payload PayLoad meshPayload,
    out vertices ShadowMapOutput outVerts[128],
    out indices uint32_t3     outIndices[128]
)
{
    // Meshlet取得
    Meshlet meshlet = mMeshlets[meshPayload.groupID];

    // メッシュレット出力数を求める
    SetMeshOutputCounts(meshlet.VertCount, meshlet.PrimCount);

    if (gtid < meshlet.VertCount) {
        // 頂点インデックスの取得
        uint32_t vertexIndex = GetVertexIndex(meshlet, gtid);
        
        // 取得したインデックスから頂点座標を求める
        Vertex vertex = mVertices[vertexIndex];
        
        // 出力する頂点座標を求める
        outVerts[gtid].position = mul(mul(Skinning(vertex), InstData[gid].wtf.m), gViewProjection.m);
    }
    if (gtid < meshlet.PrimCount)
    {
        // プリミティブ情報のインデックス情報を取得
        uint32_t3 packedIndices = GetPrimitive(meshlet, gtid);
        
        // 出力するプリミティブを求める
        outIndices[gtid] = packedIndices;
    }

}