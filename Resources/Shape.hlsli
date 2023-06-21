struct TransformationMatrix {
    float32_t4x4 WVP;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

struct VertexShaderInput {
    float4 position : POSITION0;
    float4 color : COLOR0;
};