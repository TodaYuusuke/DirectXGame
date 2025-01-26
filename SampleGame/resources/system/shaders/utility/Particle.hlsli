/// <summary>
/// Particle Struct
/// </summary>

struct ParticleData
{
    float32_t3 translate;
    float32_t3 scale;
    float32_t3 velocity;
    float32_t lifeTime;
};

struct EmitterSphere
{
    float32_t3 position;
    float32_t radius;
    uint32_t count;
    float32_t frequency;
    float32_t frequencyTime;
    uint32_t emit;   
};