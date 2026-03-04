#include "Map/Particle.h"

namespace nyaa {

Particle::Particle()
{
    reset();
}

void Particle::reset()
{
    x = y = z = 0.0f;
    vx = vy = vz = 0.0f;
    size = 1.0f;
    rotation = 0.0f;
    rotationSpeed = 0.0f;
    lifetime = 0.0f;
    maxLifetime = 1.0f;
    r = g = b = a = 1.0f;
    frame = 0;
    active = false;
}

} // namespace nyaa