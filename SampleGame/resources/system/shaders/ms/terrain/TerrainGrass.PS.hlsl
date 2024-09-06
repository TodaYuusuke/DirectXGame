#include "TerrainGrass.hlsli"

float32_t4 main(VSOutput input) : SV_TARGET
{
    return input.color;
}