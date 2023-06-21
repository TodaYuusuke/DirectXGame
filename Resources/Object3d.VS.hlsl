#include "Shape.hlsli"

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.position = input.position;
    output.color = input.color;
    return output;
}