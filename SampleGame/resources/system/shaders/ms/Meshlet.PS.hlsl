#include "Meshlet.hlsli"

// 影の濃さ
const static float kShadowDensity = 0.2f;

// -- 平行光源のライティング -- //
float3 DirectionLightingDiffuse(VSOutput input, uint32_t n)
{
    // 拡散反射
    float NdotL = dot(normalize(input.normal), -cDLights[n].direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    return cDLights[n].color.rgb * cos * cDLights[n].intensity;
}
float3 DirectionLightingSpecular(VSOutput input, uint32_t n, float3 toEye, uint32_t m)
{
    // 鏡面反射
    float32_t3 halfVector = normalize(-cDLights[n].direction + toEye);
    float NdotH = dot(normalize(input.normal), halfVector);
    float specularPow = pow(saturate(NdotH), cMaterials[m].shinines); // 反射強度
    return cDLights[n].color.rgb * cDLights[n].intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
}
float3 DirectionLightingShadow(VSOutput input, uint32_t n)
{
    // シャドウマッピング
    float3 posSM = mul(float4(input.worldPos, 1.0f), cDLights[n].viewProjection).xyz;
    posSM.x = (1.0f + posSM.x) / 2.0f;
    posSM.y = (1.0f - posSM.y) / 2.0f;
    float depth = tDLightSM[0].Sample(sDLightSMSmp, posSM.xy);
    return (posSM.z - 0.00005f < depth) ? 1.0f : kShadowDensity;
}


// -- 点光源のライティング -- //
float3 PointLightingDiffuse(VSOutput input, uint32_t n, float3 dir)
{
    // 拡散反射
    float NdotL = dot(normalize(input.normal), -normalize(dir));
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float distance = length(cPLights[n].position - input.worldPos); // ポイントライトへの距離
    float factor = pow(saturate(-distance / cPLights[n].radius + 1.0), cPLights[n].decay); // 逆二乗則による減衰係数
    return cPLights[n].color.rgb * cos * cPLights[n].intensity * factor;
}
float3 PointLightingSpecular(VSOutput input, uint32_t n, float3 dir, float3 toEye, uint32_t m)
{
    // 鏡面反射
    float32_t3 halfVector = normalize(-dir + toEye);
    float NdotH = dot(normalize(input.normal), halfVector);
    float specularPow = pow(saturate(NdotH), cMaterials[m].shinines); // 反射強度
    return cPLights[n].color.rgb * cPLights[n].intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
}
float3 PointLightingShadow(VSOutput input, uint32_t n, float3 dir)
{
    // シャドウマッピング
    const float far = 300.0f; // パースペクティブのfar
    const float near = 0.01f; // パースペクティブのnear
    
    float depth = tPLightSM[n].Sample(sPLightSMSmp, normalize(dir)).x;
    float3 absVec = abs(dir);
    float z = max(absVec.x, max(absVec.y, absVec.z));
    float normZComp = far / (far - near) - (far * near) / (far - near) / z;
    return (normZComp - 0.00005f < depth) ? 1.0f : kShadowDensity;
}



float32_t4 main(VSOutput input) : SV_TARGET
{
    // インデックス抽出
    int32_t m = input.mIndex;
    int32_t t = cMaterials[m].tIndex;
    
    // 最終的な出力結果
    float32_t4 output = { 0.0f, 0.0f, 0.0f, 0.0f };
    // カメラへの視点
    float3 toEye = normalize(cCamera.position - input.worldPos);
    
    if (cMaterials[m].enableLighting != 0)
    { // Lightingの計算を行う
        // -- テクスチャ -- //
        // uvTransform
        float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), cMaterials[m].uvTransform);
        // テクスチャからサンプリング
        float4 texColor = tTexture[t].Sample(sTexSmp, transformUV.xy);


        // 必要な変数を宣言
        float3 diffuse = { 0.0f, 0.0f, 0.0f }; // 最終的な拡散反射の値
        float3 specular = { 0.0f, 0.0f, 0.0f }; // 最終的な鏡面反射の値
        float3 shadow = { 0.0f, 0.0f, 0.0f }; // 最終的なシャドウマップの値
        uint n = 0; // ループ用変数

        // -- 平行光源 -- //
        for (n = 0; n < cCommonData.directionLightCount; n++)
        {
            diffuse += DirectionLightingDiffuse(input, n);
            //specular += DirectionLightingSpecular(input, n, toEye, m);
            shadow += DirectionLightingShadow(input, n);
        }
        
        // -- 点光源 -- //
        for (n = 0; n < cCommonData.pointLightCount; n++)
        {
            float3 dir = input.worldPos - cPLights[n].position;
            diffuse += PointLightingDiffuse(input, n, dir);
            //specular += PointLightingSpecular(input, n, dir, toEye, m);
            shadow += PointLightingShadow(input, n, dir);
        }
    
        output.rgb = ((input.color.rgb * texColor.rgb * cMaterials[m].color.rgb * diffuse) + specular) * shadow;
        //output.rgb = (input.color.rgb * texColor.rgb * diffuse);
        output.a = input.color.a * texColor.a * cMaterials[m].color.a; // 透明度を保持
    }
    else
    { // Lightingの計算を行わない
        float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), cMaterials[m].uvTransform);
        output = input.color * tTexture[t].Sample(sTexSmp, transformUV.xy) * cMaterials[m].color;
    }
    
    // 計算結果を返す
    return output;
}