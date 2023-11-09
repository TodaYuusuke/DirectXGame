#include "Object3d.hlsli"

//// 方向ベクトルからキューブマップのテクスチャ座標を計算
//float2 CalculateCubeMapCoords(float3 direction)
//{
//    float3 absVec = abs(direction);
//
//    float2 cubeMapCoords;
//
//    // X軸方向
//    if (absVec.x >= absVec.y && absVec.x >= absVec.z)
//    {
//        cubeMapCoords.x = (direction.x > 0.0f) ? -direction.z : direction.z;
//        cubeMapCoords.y = -direction.y;
//    }
//    // Y軸方向
//    else if (absVec.y >= absVec.x && absVec.y >= absVec.z)
//    {
//        cubeMapCoords.x = direction.x;
//        cubeMapCoords.y = (direction.y > 0.0f) ? direction.z : -direction.z;
//    }
//    // Z軸方向
//    else
//    {
//        cubeMapCoords.x = direction.x;
//        cubeMapCoords.y = -direction.y;
//    }
//
//    return 0.5f * (cubeMapCoords + 1.0f);
//}
//
//// 方向ベクトルから対応するキューブマップのテクスチャを選択
//uint SelectCubeMapTexture(float3 direction, uint pointLightIndex)
//{
//    float2 cubeMapCoords = CalculateCubeMapCoords(direction);
//
//    // 選択されたキューブマップのインデックス
//    uint selectedTextureIndex = (uint)(floor(cubeMapCoords.x * 6.0f + 0.5f));
//    selectedTextureIndex = clamp(selectedTextureIndex, 0, 5);
//    return gPointShadowMap[pointLightIndex * 6 + selectedTextureIndex].Sample(gPointShadowMapSampler, cubeMapCoords);
//}



float32_t4 main(VertexShaderOutput input) : SV_TARGET {
    // インデックスを抽出
    uint32_t m = gIndex[input.id].material;
    uint32_t t = gIndex[input.id].tex2d;


    // 最終的な結果
    float32_t4 output;

    if (gMaterial[m].enableLighting != 0) { // Lightingの計算を行う
        // -- テクスチャ -- //
        // uvTransform
        float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial[m].uvTransform);
        // テクスチャからサンプリング
        float4 texColor = gTexture[t].Sample(gSampler, transformUV.xy);


        // 必要な変数を宣言
        float3 lighting = { 0.0f,0.0f,0.0f }; // 最終的なライティングの値

        {   // -- 平行光源 -- //
            // ライティング
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float3 light = gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            // シャドウマッピング
            float depth = gDirectionShadowMap[0].Sample(gDirectionShadowMapSampler, input.posSM.xy);
            float sma = (input.posSM.z - 0.00000075f < depth) ? 1.0f : kShadowDensity;
            // 結果を加算
            lighting += light * sma;
        }

        // -- 点光源 -- //

        const float far = 100.0f;  // パースペクティブのfar
        const float near = 0.10;  // パースペクティブのnear
        for (uint n = 0; n < gStructCount.pointLight; n++) {
            // ライティング
            float3 dir = input.WorldPos - gPointLight[n].position;
            float NdotL = dot(normalize(input.normal), -normalize(dir));
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float distance = length(gPointLight[n].position - input.WorldPos);   // ポイントライトへの距離
            float factor = pow(saturate(-distance / gPointLight[n].radius + 1.0), gPointLight[n].decay);    // 逆二乗則による減衰係数
            float3 light = gPointLight[n].color.rgb * cos * gPointLight[n].intensity * factor;
            // シャドウマッピング
            //float depth = gPointShadowMap[n].Sample(gPointShadowMapSampler, dir).x;
            //float32_t4x4 view = {
            //    {0.000796274282f,0.0f,0.999999702f,0.0f},
            //    {0.0f,1.0f,0.0f,0.0f},
            //    {-0.999999702f,0.0f,0.000796274282f,0.0f},
            //    {-0.0995859057f,0.0f,0.520079434f,1.0f}
            //};
            //float32_t4x4 projection = {
            //    {4.36919022f,0.0f,0.0f,0.0f},
            //    {0.0f,4.36919022f,0.0f,0.0f},
            //    {0.0f,0.0f,1.00100100f,1.0f},
            //    {0.0f,0.0f,-0.100100100f,0.0f}
            //};
            //float4 Pos = mul(float4(input.WorldPos, 1.0f), gLightViewProjection[n + 1]);
            //Pos.xyz = Pos.xyz / Pos.w;
            //float z = 100.0f / (100.0f - 0.10f) - (100.0f * 0.10f) / (100.0f - 0.10f) / Pos.z;
            //float sma = (z < depth) ? 1.0f : kShadowDensity;

            // 現状維持
            float depth = gPointShadowMap[n].Sample(gPointShadowMapSampler, normalize(dir)).x;
            float3 absVec = abs(dir);
            float z = max(absVec.x, max(absVec.y, absVec.z));
            float normZComp = far / (far - near) - (far * near) / (far - near) / z;
            float sma = (normZComp - 0.00000075f < depth)  ? 1.0f : kShadowDensity;
            // 結果を加算
            lighting += light * sma;
        }
        

        output.rgb = input.color.rgb * texColor.rgb * lighting;
        output.w = input.color.a * texColor.a;   // 透明度を保持
    }
    else { // Lightingの計算を行わない
        float4 transformUV = mul(float32_t4(input.texcoord,0.0f,1.0f), gMaterial[m].uvTransform);
        output = input.color * gTexture[t].Sample(gSampler, transformUV.xy);
    }
    return output;
}