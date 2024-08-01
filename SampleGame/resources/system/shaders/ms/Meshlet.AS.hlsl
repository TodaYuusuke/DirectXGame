#include "Meshlet.hlsli"

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    PayLoad p;
    uint32_t instanceID = DTid.x / 211;
    uint32_t meshletID = DTid.x % 211;
    p.groupID = DTid.x;
    bool visible = !(instanceID == 1 && meshletID == 3);
    //bool visible = instanceID != 1;
    
    DispatchMesh(visible, 1, 1, p);
    //DispatchMesh(mCommonData.instanceSize * visible, 1, 1, p);
}