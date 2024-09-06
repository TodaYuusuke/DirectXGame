#include "TerrainGrass.hlsli"
#include "../UtilityFunction.hlsli"

float32_t4 ConvertVector4(float32_t3 v)
{
    return float32_t4(v.x, v.y, v.z, 1.0f);
}

struct Vertices
{
    float32_t4 v[4];
};

// 点に草を生やす
Vertices GenerateGrass(float32_t3 pos, float32_t height, float32_t2 dir)
{
    Vertices result;
    result.v[0] = mul(ConvertVector4(pos + float32_t3(-dir.x, 0.0f, -dir.y)), cCamera.viewProjection); // 左下
    result.v[1] = mul(ConvertVector4(pos + float32_t3(dir.x, 0.0f, dir.y)), cCamera.viewProjection); // 右下
    result.v[2] = mul(ConvertVector4(pos + float32_t3(-dir.x, height, -dir.y)), cCamera.viewProjection); // 左上
    result.v[3] = mul(ConvertVector4(pos + float32_t3(dir.x, height, dir.y)), cCamera.viewProjection); // 右上
    return result;
}

static const int GRASS_COUNT = 5;
static const int GRASS_VERT_COUNT = 4 * GRASS_COUNT;
static const int GRASS_PRIM_COUNT = 2 * GRASS_COUNT;
static const float4 GRASS_COLOR = float4(0.254f, 0.539f, 0.054f, 1.0f);


[NumThreads(1, 1, 1)]
[OutputTopology("triangle")]
void main(
    in uint gid : SV_GroupID, // グループID
    in uint gtid : SV_GroupThreadID, // グループ内のスレッドID
    out vertices VSOutput outVerts[GRASS_VERT_COUNT], // 出力頂点
    out indices uint3 outIndices[GRASS_PRIM_COUNT] // 出力インデックス
)
{
    // 頂点数とプリミティブ（三角形）の数を設定
    SetMeshOutputCounts(GRASS_VERT_COUNT, GRASS_PRIM_COUNT);
    // 中央位置を求める
    float32_t3 center = sInstance[gid].position;

    // 平面を構成する4頂点の位置を計算
    float halfSize = 0.5f;
    
    // 草を生成
    RandomGenerator random;
    random.seed = center;   // seed生成
    for (int i = 0; i < GRASS_COUNT; i++)
    {
        Vertices ver;
        float32_t2 dir = normalize(float32_t2(2 * random.Generate1d() - 1, 2 * random.Generate1d() - 1));
        dir *= 0.02f;
        //float32_t2 dist = normalize(float32_t2(2 * random.Generate1d() - 1, 2 * random.Generate1d() - 1));
        //dist *= random.Generate1d() * 0.2f;
        float32_t2 dist = float32_t2(random.Generate1d() * 0.3f, random.Generate1d() * 0.3f);
        ver = GenerateGrass(center + float3(dist.x, 0.0f, dist.y), 0.1f + random.Generate1d() * 0.08f, dir);
        
        int vOffset = i * 4;
        
        for (int j = 0; j < 4; j++)
        {
            outVerts[j + vOffset].position = ver.v[j];
            outVerts[j + vOffset].normal = float3(0.0f, 1.0f, 0.0f);
            outVerts[j + vOffset].color = GRASS_COLOR;
        }
        
        // インデックスを設定して三角形を構成
        outIndices[i*2]     = uint3(0 + vOffset, 2 + vOffset, 1 + vOffset); // 左下三角形
        outIndices[i*2 + 1] = uint3(1 + vOffset, 2 + vOffset, 3 + vOffset); // 右上三角形
    }
    
    // 中心にのみ草を生やす処理（GRASS_COUNTを1にすること）
    /*
    RandomGenerator random;
    random.seed = center; // seed生成
    float32_t2 dir = normalize(float32_t2(2 * random.Generate1d() - 1, 2 * random.Generate1d() - 1));
    dir *= 0.02f;
    Vertices ver = GenerateGrass(center, 0.4f, dir);
    
    for (int j = 0; j < 4; j++)
    {
        outVerts[j].position = ver.v[j];
        outVerts[j].normal = float3(0.0f, 1.0f, 0.0f);
        outVerts[j].color = GRASS_COLOR;
    }
        
        // インデックスを設定して三角形を構成
    outIndices[0] = uint3(0, 2, 1); // 左下三角形
    outIndices[1] = uint3(1, 2, 3); // 右上三角形
    */
}