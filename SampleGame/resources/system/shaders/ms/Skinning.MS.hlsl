#include "Meshlet.hlsli"
struct Skinned {
    float32_t4 position;
    float32_t3 normal;
};

StructuredBuffer<Well> Wells : register(t517);

Skinned Skinning(Vertex v, uint32_t instanceIndex)
{
    Skinned skinned;
    
    skinned.position = mul(v.position, Wells[v.jIndex.x + (instanceIndex * mCommonData.jSize)].skeletonSpaceMatrix) * v.weight.x;
    skinned.position += mul(v.position, Wells[v.jIndex.y + (instanceIndex * mCommonData.jSize)].skeletonSpaceMatrix) * v.weight.y;
    skinned.position += mul(v.position, Wells[v.jIndex.z + (instanceIndex * mCommonData.jSize)].skeletonSpaceMatrix) * v.weight.z;
    skinned.position += mul(v.position, Wells[v.jIndex.w + (instanceIndex * mCommonData.jSize)].skeletonSpaceMatrix) * v.weight.w;
    skinned.position.w = 1.0f;
    
    skinned.normal = mul(v.normal, (float32_t3x3) Wells[v.jIndex.x + (instanceIndex * mCommonData.jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.x;
    skinned.normal += mul(v.normal, (float32_t3x3) Wells[v.jIndex.y + (instanceIndex * mCommonData.jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.y;
    skinned.normal += mul(v.normal, (float32_t3x3) Wells[v.jIndex.z + (instanceIndex * mCommonData.jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.z;
    skinned.normal += mul(v.normal, (float32_t3x3) Wells[v.jIndex.w + (instanceIndex * mCommonData.jSize)].skeletonSpaceInverseTransposeMatrix) * v.weight.w;
    skinned.normal = normalize(skinned.normal);
    
    return skinned;
}

[NumThreads(128, 1, 1)]      // スレッド数最大128
[OutputTopology("triangle")] // 出力形状は三角形
void main(
    in uint32_t gid  : SV_GroupID,
    in uint32_t gtid : SV_GroupThreadID,
    in payload PayLoad meshPayload,
    out vertices VSOutput outVerts[128],
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
        
        // 出力する頂点のデータを求める
        Skinned skinned = Skinning(vertex, gid);
        //outVerts[gtid].pos = mul(mul(vertex.position, InstanceData.wtf.m), cCamera.viewProjection);
        outVerts[gtid].pos = mul(mul(skinned.position, InstData[gid].wtf.m), cCamera.viewProjection);
        //outVerts[gtid].worldPos = mul(vertex.position, InstanceData.wtf.m).xyz;
        outVerts[gtid].worldPos = mul(skinned.position, InstData[gid].wtf.m).xyz;
        outVerts[gtid].texcoord = vertex.texcoord;
        //outVerts[gtid].normal = normalize(mul(vertex.normal, transpose((float32_t3x3) InstanceData.wtf.inverse)));
        outVerts[gtid].normal = normalize(mul(skinned.normal, transpose((float32_t3x3) InstData[gid].wtf.inverse)));
        outVerts[gtid].color = vertex.color;
        outVerts[gtid].mIndex = vertex.mIndex;
    }
    if (gtid < meshlet.PrimCount)
    {
        // プリミティブ情報のインデックス情報を取得
        uint32_t3 packedIndices = GetPrimitive(meshlet, gtid);
        
        // 出力するプリミティブを求める
        outIndices[gtid] = packedIndices;
    }
}