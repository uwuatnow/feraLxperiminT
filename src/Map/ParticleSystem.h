#pragma once
#include "Game/Globals.h"
#include "Map/Particle.h"
#include <vector>

namespace nyaa {

/**
 * @brief Particle system for managing and rendering particles
 * 
 * Uses object pooling for performance - particles are pre-allocated
 * and reused rather than created/destroyed at runtime.
 */
class ParticleSystem
{
public:
    static constexpr int MaxParticles = 500;  // Maximum number of particles
    static constexpr int ParticlesPerImpact = 8;  // Particles per bullet impact

public:
    ParticleSystem();
    ~ParticleSystem() = default;

public:
    /**
     * @brief Emit particles at a position (e.g., bullet impact)
     * @param x World X position
     * @param y World Y position (depth)
     * @param z World Z position (height)
     * @param count Number of particles to emit
     * @param speed Multiplier for particle velocity
     */
    void emit(float x, float y, float z, int count = ParticlesPerImpact, float speed = 1.0f);

    /**
     * @brief Emit spark particles for bullet impacts
     * @param x World X position
     * @param y World Y position
     * @param z World Z position
     * @param normalX Surface normal X (for direction bias)
     * @param normalY Surface normal Y
     */
    void emitSparks(float x, float y, float z, float normalX = 0.0f, float normalY = 0.0f);

    /**
     * @brief Update all active particles
     * @param deltaTime Time since last frame in seconds
     */
    void update(float deltaTime);

    /**
     * @brief Get particle count for rendering
     * Counts active particles directly to ensure accurate count before update() is called
     */
    int getParticleCount() const;

    /**
     * @brief Get particle data for rendering
     */
    const Particle* getParticles() const { return m_Particles.data(); }

    /**
     * @brief Clear all particles
     */
    void clear() { m_ParticleCount = 0; }

private:
    /**
     * @brief Find an inactive particle in the pool
     */
    Particle* findFreeParticle();

    /**
     * @brief Initialize a particle with spark-like properties
     */
    void initSparkParticle(Particle* p, float x, float y, float z, float speed);

    std::vector<Particle> m_Particles;  // Particle pool
    int m_ParticleCount;  // Number of active particles
};

} // namespace nyaa