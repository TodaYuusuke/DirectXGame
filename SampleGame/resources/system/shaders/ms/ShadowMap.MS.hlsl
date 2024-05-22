#include "MSStruct.hlsli"

struct ShadowMapOutput
{
    float4 position : SV_POSITION;
};
struct ViewProjection
{
    float32_t4x4 m;
};

ConstantBuffer<InstanceData> InstanceData : register(b0);
ConstantBuffer<ViewProjection> gViewProjection : register(b1);

// Required
StructuredBuffer<Meshlet> Meshlets : register(t0);
StructuredBuffer<Vertex> Vertices : register(t1);
ByteAddressBuffer UniqueVertexIndices : register(t2);
StructuredBuffer<uint32_t> PrimitiveIndices : register(t3);


uint32_t3 UnpackPrimitive(uint primitive) {
    // 10ビットごとにプリミティブのインデックスを読み取る
    return uint32_t3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}

uint32_t3 GetPrimitive(Meshlet m, uint index) {
    // 面情報取得
    return UnpackPrimitive(PrimitiveIndices[m.PrimOffset + index]);
}

uint32_t GetVertexIndex(Meshlet m, uint localIndex) {
    // 頂点のインデックスを求める
    localIndex = m.VertOffset + localIndex;
    // 4バイトずつで読み込む
    return UniqueVertexIndices.Load(localIndex * 4);
}

[NumThreads(128, 1, 1)]      // スレッド数最大128
[OutputTopology("triangle")] // 出力形状は三角形
void main(
    in uint32_t gid  : SV_GroupID,
    in uint32_t gtid : SV_GroupThreadID,
    out vertices ShadowMapOutput outVerts[128],
    out indices uint32_t3     outIndices[128]
)
{
    // Meshlet取得
    Meshlet meshlet = Meshlets[gid];

    // メッシュレット出力数を求める
    SetMeshOutputCounts(meshlet.VertCount, meshlet.PrimCount);

    if (gtid < meshlet.VertCount) {
        // 頂点インデックスの取得
        uint32_t vertexIndex = GetVertexIndex(meshlet, gtid);
        
        // 取得したインデックスから頂点座標を求める
        Vertex vertex = Vertices[vertexIndex];
        
        // 出力する頂点座標を求める
        outVerts[gtid].position = mul(mul(vertex.position, InstanceData.wtf.m), gViewProjection.m);
    }
    if (gtid < meshlet.PrimCount)
    {
        // プリミティブ情報のインデックス情報を取得
        uint32_t3 packedIndices = GetPrimitive(meshlet, gtid);
        
        // 出力するプリミティブを求める
        outIndices[gtid] = packedIndices;
    }

}