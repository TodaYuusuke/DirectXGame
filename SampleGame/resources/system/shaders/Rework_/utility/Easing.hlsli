float32_t InCubic(float32_t x) {
    return x * x * x;
}

float32_t3 Lerp(float32_t3 start, float32_t3 end, float32_t t) {
    return (1.0f - t) * start + end * t;
}
