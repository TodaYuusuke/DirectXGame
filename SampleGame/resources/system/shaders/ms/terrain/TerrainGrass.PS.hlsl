#include "Struct.hlsli"

struct PixelShaderOutput
{
    float3 patchPosition : SV_Target0;
    float4 baseColor : SV_Target1;
    float3 normal : SV_Target2;
};

PixelShaderOutput GrassPatchPixelShader(const Vertex input, bool isFrontFace : SV_IsFrontFace)
{
    PixelShaderOutput output;
    output.position = input.worldSpacePosition;
    	
    float selfshadow = clamp(pow((input.worldSpacePosition.y - input.rootHeight) / input.height, 1.5), 0, 1);
    output.baseColor.rgb = pow(float3(0.41, 0.44, 0.29), 2.2) * selfshadow;
    output.baseColor.rgb *= 0.75 + 0.25 * PerlinNoise2D(0.25 * input.worldSpacePosition.xy);
    output.baseColor.a = 1;
    
    float3 normal = normalize(input.worldSpaceNormal);

    if (!isFrontFace)
    {
        normal = -normal;
    }

    output.normal.xyz = normalize(lerp(float3(0, 0, 1), normal, 0.25));

    return output;
}