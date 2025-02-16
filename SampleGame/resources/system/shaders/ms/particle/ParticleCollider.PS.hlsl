#include "Particle.hlsli"

float32_t4 main(ParticleColliderOutput input) : SV_TARGET
{
    float32_t r = (input.id & 0xFF) / 255.0f; // 下位8bit
    float32_t g = ((input.id >> 8) & 0xFF) / 255.0f; // 次の8bit
    float32_t b = ((input.id >> 16) & 0xFF) / 255.0f; // 上位8bit (今回は不要)
    return float32_t4(r, g, b, 1.0f); // RGBAに格納
}