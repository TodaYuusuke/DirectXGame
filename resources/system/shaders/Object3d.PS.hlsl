#include "Object3d.hlsli"

// -- 平行光源のライティング -- //
float3 DirectionLightingDiffuse(VertexShaderOutput input, uint32_t n) {
    // 拡散反射
    float NdotL = dot(normalize(input.normal), -gDirectionalLight[n].direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    return gDirectionalLight[n].color.rgb * cos * gDirectionalLight[n].intensity;
}
float3 DirectionLightingSpecular(VertexShaderOutput input, uint32_t n, float3 toEye, uint32_t m) {
    // 鏡面反射
    float32_t3 halfVector = normalize(-gDirectionalLight[n].direction + toEye);
    float NdotH = dot(normalize(input.normal), halfVector);
    float specularPow = pow(saturate(NdotH), gMaterial[m].shinines); // 反射強度
    return gDirectionalLight[n].color.rgb * gDirectionalLight[n].intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
}
float3 DirectionLightingShadow(VertexShaderOutput input, uint32_t n) {
    // シャドウマッピング
    float3 posSM = mul(float4(input.worldPos, 1.0f), gDirectionalLight[n].viewProjection).xyz;
    posSM.x = (1.0f + posSM.x) / 2.0f;
    posSM.y = (1.0f - posSM.y) / 2.0f;
    float depth = gDirectionShadowMap[0].Sample(gDirectionShadowMapSampler, posSM.xy);
    return (posSM.z - 0.00000075f < depth) ? 1.0f : kShadowDensity;
}


// -- 点光源のライティング -- //
float3 PointLightingDiffuse(VertexShaderOutput input, uint32_t n, float3 dir) {
    // 拡散反射
    float NdotL = dot(normalize(input.normal), -normalize(dir));
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float distance = length(gPointLight[n].position - input.worldPos); // ポイントライトへの距離
    float factor = pow(saturate(-distance / gPointLight[n].radius + 1.0), gPointLight[n].decay); // 逆二乗則による減衰係数
    return gPointLight[n].color.rgb * cos * gPointLight[n].intensity * factor;
}
float3 PointLightingSpecular(VertexShaderOutput input, uint32_t n, float3 dir, float3 toEye, uint32_t m) {
    // 鏡面反射
    float32_t3 halfVector = normalize(-dir + toEye);
    float NdotH = dot(normalize(input.normal), halfVector);
    float specularPow = pow(saturate(NdotH), gMaterial[m].shinines); // 反射強度
    return gPointLight[n].color.rgb * gPointLight[n].intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
}
float3 PointLightingShadow(VertexShaderOutput input, uint32_t n, float3 dir) {
    // シャドウマッピング
    const float far = 100.0f; // パースペクティブのfar
    const float near = 0.01f; // パースペクティブのnear
    
    float depth = gPointShadowMap[n].Sample(gPointShadowMapSampler, normalize(dir)).x;
    float3 absVec = abs(dir);
    float z = max(absVec.x, max(absVec.y, absVec.z));
    float normZComp = far / (far - near) - (far * near) / (far - near) / z;
    return (normZComp - 0.00000075f < depth) ? 1.0f : kShadowDensity;
}



float32_t4 main(VertexShaderOutput input) : SV_TARGET
{
    // インデックスを抽出
    uint32_t m = gIndex[input.id].material;
    uint32_t t = gMaterial[m].tIndex;

    // 最終的な結果
    float32_t4 output;

    // カメラへの視点
    float3 toEye = normalize(gCameraData.position - input.worldPos);
    
    
    if (gMaterial[m].enableLighting != 0)
    { // Lightingの計算を行う
        // -- テクスチャ -- //
        // uvTransform
        float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial[m].uvTransform);
        // テクスチャからサンプリング
        float4 texColor = gTexture[t].Sample(gSampler, transformUV.xy);


        // 必要な変数を宣言
        float3 diffuse = { 0.0f, 0.0f, 0.0f }; // 最終的な拡散反射の値
        float3 specular = { 0.0f, 0.0f, 0.0f }; // 最終的な鏡面反射の値
        float3 shadow = { 0.0f, 0.0f, 0.0f }; // 最終的なシャドウマップの値
        uint n = 0; // ループ用変数

        // -- 平行光源 -- //
        for (n = 0; n < gCommonData.directionLightCount; n++)
        {
            diffuse += DirectionLightingDiffuse(input, n);
            specular += DirectionLightingSpecular(input, n, toEye, m);
            shadow += DirectionLightingShadow(input, n);
        }
        
        // -- 点光源 -- //
        for (n = 0; n < gCommonData.pointLightCount; n++)
        {
            float3 dir = input.worldPos - gPointLight[n].position;
            diffuse += PointLightingDiffuse(input, n, dir);
            specular += PointLightingSpecular(input, n, dir, toEye, m);
            shadow += PointLightingShadow(input, n, dir);
        }
        
        //output.rgb = ((input.color.rgb * diffuse) + specular) * shadow;
        //output.w = input.color.a; // 透明度を保持
    
        output.rgb = ((input.color.rgb * texColor.rgb * gMaterial[m].color.rgb * diffuse) + specular) * shadow;
        output.w = input.color.a * texColor.a * gMaterial[m].color.a; // 透明度を保持
    }
    else
    { // Lightingの計算を行わない
        float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial[m].uvTransform);
        output = input.color * gTexture[t].Sample(gSampler, transformUV.xy) * gMaterial[m].color;
    }

    // 透明なら消す
    if (output.a <= 0.0f)
    {
        discard;
    }

    return output;
}