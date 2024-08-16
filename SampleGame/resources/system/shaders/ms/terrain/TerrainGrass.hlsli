#include "Struct.hlsli"

ConstantBuffer<CommonData> cCommon : register(b1);
ConstantBuffer<Camera> cCamera : register(b2);

StructuredBuffer<InstanceData> sInstance : register(t0);

