#include "../../Structs.hlsli"

Texture2D<float32_t4> tAlbedo : register(t0);
Texture2D<float32_t4> tNormal : register(t1);
Texture2D<float32_t4> tPBR : register(t2);
Texture2D<float32_t4> tWorldPosition : register(t3);
SamplerState cSampler : register(s0);

ConstantBuffer<LightMetadata> cLightMeta : register(b0);
ConstantBuffer<DirectionalLight> cDirLight : register(b1);
//StructuredBuffer<PointLight> sPointLights : register(t4);

Texture2D<float> tDirLightShadowMap : register(t4);
SamplerState sDirLightSampler : register(s1);
//TextureCube<float> tPointLightShadowMaps[] : register(t0, space1);
//SamplerState sPointLightSampler : register(s2);

// -- 平行光源のライティング -- //
float3 DirLightingDiffuse(float32_t3 normal) {
    // 拡散反射
    float NdotL = dot(normalize(normal), -cDirLight.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    return cDirLight.color.rgb * cos * cDirLight.intensity;
}/*
float3 DirLightingSpecular(PassThroughOutput input, float3 toEye, uint32_t m) {
    // 鏡面反射
    float32_t3 halfVector = normalize(-cDLights[n].direction + toEye);
    float NdotH = dot(normalize(input.normal), halfVector);
    float specularPow = pow(saturate(NdotH), cMaterials[m].shinines); // 反射強度
    return cDLights[n].color.rgb * cDLights[n].intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
}*/
float3 DirLightingShadow(float32_t3 worldPosition, float32_t3 normal) {
    // シャドウマッピング
    float32_t4 lightClip = mul(float4(worldPosition, 1.0f), cDirLight.m);
    
    float32_t2 shadowUV = (lightClip.xy / lightClip.w) * float32_t2(0.5f, -0.5f) + 0.5f;
    float32_t litDepth = lightClip.z / lightClip.w;
   
    float32_t ndcDepth = tDirLightShadowMap.Sample(sDirLightSampler, shadowUV);
    
    // 比較自体がなにか間違えている？
    return (litDepth - cDirLight.bias < ndcDepth) ? 1.0f : cDirLight.shadowIntensity;
}

// -- 点光源のライティング -- //
/*float3 PointLightingDiffuse(PassThroughOutput input, uint32_t n, float3 dir)
{
    // 拡散反射
    float NdotL = dot(normalize(input.normal), -normalize(dir));
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float distance = length(cPLights[n].position - input.worldPos); // ポイントライトへの距離
    float factor = pow(saturate(-distance / cPLights[n].radius + 1.0), cPLights[n].decay); // 逆二乗則による減衰係数
    return cPLights[n].color.rgb * cos * cPLights[n].intensity * factor;
}
float3 PointLightingSpecular(PassThroughOutput input, uint32_t n, float3 dir, float3 toEye, uint32_t m)
{
    // 鏡面反射
    float32_t3 halfVector = normalize(-dir + toEye);
    float NdotH = dot(normalize(input.normal), halfVector);
    float specularPow = pow(saturate(NdotH), cMaterials[m].shinines); // 反射強度
    return cPLights[n].color.rgb * cPLights[n].intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
}
float3 PointLightingShadow(PassThroughOutput input, uint32_t n, float3 dir) {
    // シャドウマッピング
    const float far = 300.0f; // パースペクティブのfar
    const float near = 0.01f; // パースペクティブのnear
    
    float depth = tPLightSM[n].Sample(sPLightSMSmp, normalize(dir)).x;
    float3 absVec = abs(dir);
    float z = max(absVec.x, max(absVec.y, absVec.z));
    float normZComp = far / (far - near) - (far * near) / (far - near) / z;
    return (normZComp - 0.00005f < depth) ? 1.0f : kShadowDensity;
}*/

float32_t4 main(PassThroughOutput input) : SV_TARGET {
    // 遅延ライティング用のリソースを取得
    float32_t4 albedo = tAlbedo.Sample(cSampler, input.texcoord);
    float32_t4 normal = tNormal.Sample(cSampler, input.texcoord);
    
    // ライティングがオフならばここで処理を終える（法線のwに格納されている）
    if (normal.w <= 0.0f) {
        if (albedo.a <= 0.0f) { discard; }  // 透明度が0以下ならdiscord
        return albedo;
    }
    
    // 残りの遅延ライティング用のリソースも取得
    float32_t4 pbr = tPBR.Sample(cSampler, input.texcoord);
    float32_t4 worldPosition = tWorldPosition.Sample(cSampler, input.texcoord);
    
    // 最終的な出力結果
    float32_t4 output = { 0.0f, 0.0f, 0.0f, 0.0f };
    // カメラへの視点
    //float3 toEye = normalize(cCamera.position - input.worldPos);
    
    // Lightingの計算を行う

    // 必要な変数を宣言
    float3 diffuse = { 0.0f, 0.0f, 0.0f }; // 最終的な拡散反射の値
    float3 specular = { 0.0f, 0.0f, 0.0f }; // 最終的な鏡面反射の値
    float3 shadow = { 0.0f, 0.0f, 0.0f }; // 最終的なシャドウマップの値
    uint n = 0; // ループ用変数

    // -- 平行光源 -- //
    diffuse += DirLightingDiffuse(normal.xyz);
    //specular += DirectionLightingSpecular(input, toEye, m);
    shadow += DirLightingShadow(worldPosition.xyz, normal.xyz);
        
    // -- 点光源 -- //
    /*for (n = 0; n < cCommonData.pointLightCount; n++) {
        float3 dir = input.worldPos - cPLights[n].position;
        diffuse += PointLightingDiffuse(input, n, dir);
            //specular += PointLightingSpecular(input, n, dir, toEye, m);
        shadow += PointLightingShadow(input, n, dir);
    }*/
    
    output.rgb = ((albedo.rgb * diffuse) + specular) * shadow;
    output.a = albedo.a; // 透明度を保持
    
    if (albedo.a <= 0.0f) { discard; }  // 透明度が0以下ならdiscord
    
    // 計算結果を返す
    return output;
}