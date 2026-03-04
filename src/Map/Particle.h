#pragma once
#include "Game/Globals.h"
#include "Game/Clock.h"

namespace nyaa {

/**
 * @brief Represents a single particle in the particle system
 * 
 * Particles are simple billboard sprites that can be used for various
 * visual effects like sparks, debris, blood splatter, etc.
 */
struct Particle
{
public:
    Particle();

public:
    float x, y, z;           // Position in world space
    float vx, vy, vz;        // Velocity
    float size;              // Size scale
    float rotation;          // Rotation angle in degrees
    float rotationSpeed;     // Rotation speed
    float lifetime;          // Current lifetime remaining (seconds)
    float maxLifetime;       // Maximum lifetime
    float r, g, b, a;        // Color (RGBA)
    unsigned char frame;     // Texture frame index
    bool active;             // Whether particle is alive

    /**
     * @brief Check if particle is still alive
     */
    bool isAlive() const { return active && lifetime > 0.0f; }

    /**
     * @brief Reset particle to default state
     */
    void reset();
};

} // namespace nyaa