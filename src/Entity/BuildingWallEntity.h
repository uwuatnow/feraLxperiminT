#pragma once
#include "Entity/Entity.h"
#include "Game/OBB.h"

namespace nyaa {

// A physics-enabled building wall that can be pushed over by cars
class BuildingWallEntity : public Entity {
public:
    BuildingWallEntity(double x1, double y1, double x2, double y2, float height, int textureId = 0);
    virtual ~BuildingWallEntity();

    virtual void update() override;
    virtual void hurt(float amount, DamageReason damageReason, Entity* damageSource = nullptr) override;
    virtual void playCollisionSfx() override;
    
    // Get the OBB for this wall (centered on the wall line)
    virtual class OBB* getOBB() const override;
    
    // Apply a physics push that can knock the wall over
    void applyPushForce(float forceX, float forceY, float forceMagnitude, float contactX, float contactY);
    
    // Check if the wall has been knocked over
    bool isKnockedOver() const { return knockedOver; }
    
    // Get the wall dimensions for rendering
    double getX1() const { return wallX1; }
    double getY1() const { return wallY1; }
    double getX2() const { return wallX2; }
    double getY2() const { return wallY2; }
    float getHeight() const { return wallHeight; }
    float getLength() const { return wallLength; }
    float getCurrentTiltAngle() const { return tiltAngle; }
    int getTextureId() const { return texId; }
    
    // Get the direction the wall is falling
    void getFallDirection(float* dx, float* dy) const {
        if (dx && dy) {
            *dx = fallDirX;
            *dy = fallDirY;
        }
    }
    
    // Get health for damage tracking
    float getHealth() const { return health; }
    
    // Wall thickness for collision
    static constexpr float WALL_THICKNESS = 4.0f;

private:
    double wallX1, wallY1, wallX2, wallY2;
    float wallHeight;
    int texId;
    
    // Physics state
    float health;
    float tiltAngle; // 0 = upright, 90 = knocked over
    float tiltVelocity; // degrees per second
    bool knockedOver;
    bool falling;
    
    // Pivot point for rotation (the side that's anchored to the ground when tilting)
    // Fall direction (normalized vector)
    float fallDirX, fallDirY;
    
    // Wall orientation
    float wallAngle; // Angle in degrees
    float wallLength;
    
    // Stability threshold - force required to start knocking over
    static constexpr float PUSH_FORCE_THRESHOLD = 8000.0f;
    static constexpr float DAMAGE_THRESHOLD = 15.0f;
    static constexpr float FALL_GRAVITY = 180.0f; // degrees/s^2
    static constexpr float MAX_TILT = 90.0f;
    static constexpr float INITIAL_HEALTH = 50.0f;
    
    mutable OBB* m_wallOBB;
};

extern bool disableBuildingWallPhysics;

} // namespace nyaa
