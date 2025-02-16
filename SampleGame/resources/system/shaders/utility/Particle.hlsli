/// <summary>
/// Particle Struct
/// </summary>

struct ParticleData
{
    float32_t3 translate;
    float32_t3 scale;
    float32_t3 velocity;
    float32_t4 color;
    float32_t lifeTime;
    uint32_t isActive;
    uint32_t id;
};

struct ParticleSize {
    uint32_t s;
};

struct EmitterSphere
{
    float32_t3 position;
    float32_t radius;
    float32_t frequency;
    float32_t frequencyTime;
};
