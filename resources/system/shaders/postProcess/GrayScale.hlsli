#include "PassThrough.hlsli"

float32_t3 GrayScale(float32_t3 color) {
    return dot(color, float32_t3(0.2125f, 0.7154f, 0.0721f));
}