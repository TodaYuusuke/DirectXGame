#include "Particle.hlsli"

float32_t4x4 GetIndentity4x4() {
    return float32_t4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
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
    
    // パーティクルがfalseなら出力しない
    uint32_t vertexCount = sParticle[gid].isActive ? meshlet.VertCount : 0;
    uint32_t primitiveCount = sParticle[gid].isActive ? meshlet.PrimCount : 0;
    
    // メッシュレット出力数を求める
    SetMeshOutputCounts(vertexCount, primitiveCount);
    
    // 早期リターン
    if (!sParticle[gid].isActive) { return; }

    if (gtid < meshlet.VertCount) {
        // 頂点インデックスの取得
        uint32_t vertexIndex = GetVertexIndex(meshlet, gtid);
        
        // 取得したインデックスから頂点座標を求める
        Vertex vertex = mVertices[vertexIndex];
        
        // パーティクルから行列生成
        ParticleData particle = sParticle[gid];
        float32_t4x4 worldMatrix = GetIndentity4x4();
        worldMatrix[0] *= particle.scale.x;
        worldMatrix[1] *= particle.scale.y;
        worldMatrix[2] *= particle.scale.z;
        worldMatrix[3].xyz = particle.translate;
        
        // 出力する頂点のデータを求める
        outVerts[gtid].pos = mul(mul(vertex.position, worldMatrix), cCamera.viewProjection);
        outVerts[gtid].worldPos = mul(vertex.position, worldMatrix).xyz;
        outVerts[gtid].texcoord = vertex.texcoord;
        outVerts[gtid].normal = vertex.normal;
        //outVerts[gtid].normal = normalize(mul(vertex.normal, transpose((float32_t3x3)XMMatrixInverse(worldMatrix))));
        outVerts[gtid].color = particle.color;
        outVerts[gtid].mIndex = vertex.mIndex;
        //outVerts[gtid].mIndex = vertex.mIndex + (gid * mCommonData.mSize); // マテリアルのインデックスをインスタンス番号分ずらす
    }
    if (gtid < meshlet.PrimCount)
    {
        // プリミティブ情報のインデックス情報を取得
        uint32_t3 packedIndices = GetPrimitive(meshlet, gtid);
        
        // 出力するプリミティブを求める
        outIndices[gtid] = packedIndices;
    }

}