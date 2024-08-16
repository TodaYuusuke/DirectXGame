#include "Struct.hlsli"
#include "../UtilityFunction.hlsli"

int tsign(in uint gtid, in int id)
{
    return (gtid & (1u << id)) ? 1 : -1;
}

static const int GROUP_SIZE = 128;
static const int GRASS_VERT_COUNT = 256;
static const int GRASS_PRIM_COUNT = 192;
static const int GRASS_END_DISTANCE = 1;

[NumThreads(GROUP_SIZE, 1, 1)]
[OutputTopology("triangle")]
void MeshShader(
    uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID,
    out indices uint3 tris[GRASS_PRIM_COUNT],
    out vertices Vertex verts[GRASS_VERT_COUNT]
)
{
    const GrassPatchArguments arguments = sGrassPatch[gid]; //Load arguments
    
    SetMeshOutputCounts(GRASS_VERT_COUNT, GRASS_PRIM_COUNT);
	
    static const int verticesPerBladeEdge = 4;
    static const int verticesPerBlade = 2 * verticesPerBladeEdge;
    static const int trianglesPerBlade = 6;
    static const int maxBladeCount = 32;
    
    const float3 patchCenter = arguments.position;
    const float3 patchNormal = arguments.normal;
    const float spacing = 1.0f; /*DynamicConst.grassSpacing;*/
    //const int seed = combineSeed(asuint(int(patchCenter.x / spacing)), asuint(int(patchCenter.y / spacing)));
    RandomGenerator random;
    random.seed = arguments.position / spacing;
	
    float distanceToCamera = distance(arguments.position, cCamera.position.xyz);
    float bladeCountF = lerp(float(maxBladeCount), 2., pow(saturate(distanceToCamera / (GRASS_END_DISTANCE * 1.05)), 0.75));
    
    int bladeCount = ceil(bladeCountF);
    
    const int vertexCount = bladeCount * verticesPerBlade;
    const int triangleCount = bladeCount * trianglesPerBlade;
        
    for (uint i = 0; i < 2; ++i)
    {
        int vertId = gtid + GROUP_SIZE * i;

        if (vertId >= vertexCount)
            break;
        
        int bladeId = vertId / verticesPerBlade;
        int vertIdLocal = vertId % verticesPerBlade;
           
        //const float height = arguments.height + float(rand(seed, bladeId, 20)) / 40.;
        const float height = arguments.height + random.Generate1d() / 40.;
        
        //position the grass in a circle around the patchPosition and angled using the patchNormal
        float3 tangent = normalize(cross(float3(0, 1, 0), patchNormal));
        float3 bitangent = normalize(cross(patchNormal, tangent));
             
        //float bladeDirectionAngle = 2. * PI * rand(seed, 4, bladeId);
        float bladeDirectionAngle = 2. * PI * random.Generate1d();
        float2 bladeDirection = float2(cos(bladeDirectionAngle), sin(bladeDirectionAngle));
       
        //float offsetAngle = 2. * PI * rand(seed, bladeId);
        float offsetAngle = 2. * PI * random.Generate1d();
        //float offsetRadius = spacing * sqrt(rand(seed, 19, bladeId));
        float offsetRadius = spacing * sqrt(random.Generate1d());
        float3 bladeOffset = offsetRadius * (cos(offsetAngle) * tangent + sin(offsetAngle) * bitangent);
        
        float3 p0 = patchCenter + bladeOffset;
        float3 p1 = p0 + float3(0, 0, height);
        float3 p2 = p1 + bladeDirection * height * 0.3;
     
        //p2 += GetWindOffset(p0.xy, DynamicConst.shaderTime);
        p2 += GetWindOffset(p0.xy, 1.0f);
        
        MakePersistentLength(p0, p1, p2, height);
        
        float width = 0.03;
        
        width *= maxBladeCount / bladeCountF;
        
        if (bladeId == (bladeCount - 1))
        {
            width *= frac(bladeCountF);
        }
        
        Vertex vertex;
        vertex.height = arguments.height;
        vertex.worldSpaceGroundNormal = arguments.normal;
        vertex.rootHeight = p0.z;
        
        float3 sideVec = normalize(float3(bladeDirection.y, -bladeDirection.x, 0));
        float3 offset = tsign(vertIdLocal, 0) * width * sideVec;
        
        p0 += offset * 1.0;
        p1 += offset * 0.7;
        p2 += offset * 0.3;
          
        float t = (vertIdLocal / 2) / float(verticesPerBladeEdge - 1);
        vertex.worldSpacePosition = bezier(p0, p1, p2, t);
        vertex.worldSpaceNormal = cross(sideVec, normalize(bezierDerivative(p0, p1, p2, t)));
        vertex.clipSpacePosition = mul(DynamicConst.viewProjectionMatrix, float4(vertex.worldSpacePosition, 1));

        verts[vertId] = vertex;
    }

    for (uint i = 0; i < 2; ++i)
    {
        int triId = gtid + GROUP_SIZE * i;

        if (triId >= triangleCount)
            break;

        int bladeId = triId / trianglesPerBlade;
        int triIdLocal = triId % trianglesPerBlade;

        int offset = bladeId * verticesPerBlade + 2 * (triIdLocal / 2);

        uint3 triangleIndices = (triLocal & 1) == 0 ? uint3(0, 1, 2) :
                                                     uint3(3, 2, 1);

        tris[triId] = offset + triangleIndices;
    }
}