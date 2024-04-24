// 頂点データを必要としない（フルスクリーンレンダリング前提の）PixelShader用のVertexShader
#include "PassThrough.hlsli"

PSInput main(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID) {
    // 最終的な結果
    PSInput output;

    // なんか計算ミスってる
    //float x = vertexID % 2 == 0 ? -1.0f : 1.0f;
    //float y = vertexID - 2 > 0 ? 1.0f : -1.0f;
    //output.position = float4(x, y, 0.0, 0.0);

    // 無理やり4頂点生成
    //if (vertexID == 0) {
    //    output.position = float4(-1.0, 1.0, 0.0, 0.0);
    //}
    //else if (vertexID == 1) {
    //    output.position = float4(1.0, 1.0, 0.0, 0.0);
    //}
    //else if (vertexID == 2) {
    //    output.position = float4(-1.0, -1.0, 0.0, 0.0);
    //}
    //else if (vertexID == 3) {
    //    output.position = float4(1.0, -1.0, 0.0, 0.0);
    //}

    if (vertexID == 0) {
        output.position = instanceID ? float4(-1.0, 1.0, 0.0, 1.0) : float4(-1.0, 1.0, 0.0, 1.0);
    }
    else if (vertexID == 1) {
        output.position = instanceID ? float4(1.0, 1.0, 0.0, 1.0) : float4(1.0, -1.0, 0.0, 1.0);
    }
    else if (vertexID == 2) {
        output.position = instanceID ? float4(1.0, -1.0, 0.0, 1.0) : float4(-1.0, -1.0, 0.0, 1.0);
    }

    // 座標を[0, 1]の範囲に変換（これがuv座標）
    output.texcoord = output.position.xy * 0.5 + 0.5;
    output.texcoord.y = 0.5 - (output.texcoord.y - 0.5);


    return output;
}