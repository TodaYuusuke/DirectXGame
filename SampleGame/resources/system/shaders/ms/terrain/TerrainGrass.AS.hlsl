#include "TerrainGrass.hlsli"

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    PayLoad p;
    p.groupID = DTid.x;
    DispatchMesh(cCommon.instanceSize, 1, 1, p);
}