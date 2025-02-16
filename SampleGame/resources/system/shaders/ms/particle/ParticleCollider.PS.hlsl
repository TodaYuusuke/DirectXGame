#include "Particle.hlsli"

int32_t main(ParticleColliderOutput input) : SV_TARGET
{
    return asint(input.id);
}