#include "Map/ParticleSystem.h"
#include "Game/Util.h"

namespace nyaa {

int ParticleSystem::getParticleCount() const
{
    int count = 0;
    for (const auto& p : m_Particles)
    {
        if (p.active)
            count++;
    }
    return count;
}

ParticleSystem::ParticleSystem()
    : m_Particles(MaxParticles)
    , m_ParticleCount(0)
{
    // Pre-allocate particles in the pool
    for (auto& p : m_Particles)
    {
        p.active = false;
    }
}

Particle* ParticleSystem::findFreeParticle()
{
    for (auto& p : m_Particles)
    {
        if (!p.active)
        {
            return &p;
        }
    }
    return nullptr;  // Pool exhausted
}

void ParticleSystem::initSparkParticle(Particle* p, float x, float y, float z, float speed)
{
    p->active = true;
    p->x = x;
    p->y = y;
    p->z = z;

    // Random velocity in a cone pattern (biased by surface normal if provided)
    float angle = (float)Util::RandNormalized() * 360.0f;
    float elevation = (float)Util::RandNormalized() * 60.0f - 30.0f;  // -30 to +30 degrees
    
    float vel = (0.5f + (float)Util::RandNormalized() * 1.5f) * speed;
    
    // Convert to 3D velocity
    float radAngle = angle * 3.14159f / 180.0f;
    float radElev = elevation * 3.14159f / 180.0f;
    
    p->vx = cosf(radAngle) * cosf(radElev) * vel;
    p->vy = sinf(radAngle) * cosf(radElev) * vel;
    p->vz = sinf(radElev) * vel + 0.5f;  // Slight upward bias

    p->size = 0.1f + (float)Util::RandNormalized() * 0.15f;
    p->rotation = (float)Util::RandNormalized() * 360.0f;
    p->rotationSpeed = (float)Util::RandNormalized() * 180.0f - 90.0f;
    
    p->lifetime = 0.3f + (float)Util::RandNormalized() * 0.4f;
    p->maxLifetime = p->lifetime;
    
    // Orange/yellow spark colors
    p->r = 1.0f;
    p->g = 0.6f + (float)Util::RandNormalized() * 0.4f;
    p->b = 0.2f + (float)Util::RandNormalized() * 0.3f;
    p->a = 1.0f;
    
    p->frame = (unsigned char)(Util::RandNormalized() * 4.0f);
}

void ParticleSystem::emit(float x, float y, float z, int count, float speed)
{
    for (int i = 0; i < count; ++i)
    {
        Particle* p = findFreeParticle();
        if (!p) break;  // Pool exhausted
        
        initSparkParticle(p, x, y, z, speed);
        m_ParticleCount++;
    }
}

void ParticleSystem::emitSparks(float x, float y, float z, float normalX, float normalY)
{
    // Emit sparks with a bias based on surface normal
    int count = ParticlesPerImpact;
    
    for (int i = 0; i < count; ++i)
    {
        Particle* p = findFreeParticle();
        if (!p) break;
        
        initSparkParticle(p, x, y, z, 1.0f);
        
        // Add bias based on surface normal (for wall impacts)
        if (normalX != 0.0f || normalY != 0.0f)
        {
            p->vx += normalX * 0.5f;
            p->vy += normalY * 0.5f;
        }
        
        m_ParticleCount++;
    }
}

void ParticleSystem::update(float deltaTime)
{
    m_ParticleCount = 0;
    
    for (auto& p : m_Particles)
    {
        if (!p.active) continue;
        
        // Update lifetime
        p.lifetime -= deltaTime;
        if (p.lifetime <= 0.0f)
        {
            p.active = false;
            continue;
        }
        
        // Update position
        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;
        p.z += p.vz * deltaTime;
        
        // Apply gravity
        p.vz -= 2.0f * deltaTime;
        
        // Update rotation
        p.rotation += p.rotationSpeed * deltaTime;
        
        // Fade out based on lifetime
        float lifeRatio = p.lifetime / p.maxLifetime;
        p.a = lifeRatio;
        
        // Shrink over time
        p.size *= (0.98f + 0.02f * lifeRatio);
        
        m_ParticleCount++;
    }
}

} // namespace nyaa