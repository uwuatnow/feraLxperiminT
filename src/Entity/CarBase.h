#pragma once
#include "Game/Globals.h"
#include "Entity/Entity.h"
#include "Game/Clock.h"

namespace nyaa {

class CarBase : public Entity
{
public:
    struct Wheel {
        float localX, localY; // Position relative to car center
        float steerAngle;     // Local steering angle (radians or degrees)
        bool powered;         // Is this wheel powered by engine?
        bool steerable;       // Does this wheel turn?
        
        // Physics state
        float slipAngle;      // Difference between wheel dir and velocity dir
    };

    // Virtual methods
    virtual void update() override;
    virtual void hurt(float amount, DamageReason damageReason, Entity* damageSource = nullptr) override;

    // Physics State
    float velX, velY;
    float wheelBase; // Distance between front and rear axle
    
    std::vector<Wheel> wheels;

    // Legacy/Compat (to be refactored or kept synced)
    float moveSpeed; // kept for compatibility, represents velocity magnitude roughly
    
    Entity* occupant;
    bool collisionSoundPlayed;
    float health; //max=100.0f
    
    // Visual effects
    float bodyRoll; // Body roll angle in degrees (for rendering)

    // Heights for stacking
    float hoodHeight;
    float roofHeight;

    // Fuel system
    float fuelLevel; // 0-100, percentage of fuel remaining
    bool isOutOfFuel;

    // Drift mechanics
    bool isDrifting;
    float driftTimer;
    float driftDuration;
    float driftSteeringMultiplier;
    
    // Auto-drive
    bool isAutoDriving;
    float autoDriveTargetX;
    float autoDriveTargetY;

    // Pathfinding
    Clock astClock;
    class Pathfinding* ast;
    class PathfindNode* astn;
    bool reSolveAST;
    bool directPath;
    double lastTargetX, lastTargetY;
    
    // Auto-drive methods
    bool autoDriveTo(float targetX, float targetY);
    void stopAutoDrive();

    // Stuck detection and recovery
    float autoDriveStuckTimer;
    float autoDriveReverseTimer;
    bool isAutoDriveReversing;
    bool lastFrameCollided;

    // Gear shifting
    float gearSwitchTimer;
    bool isGearReversing;

    virtual ~CarBase();


protected:
    // Protected constructor to prevent direct instantiation
    CarBase(float mass_, float inertia_, float sizeX_, float sizeY_,
             float originX_, float originY_, float friction_, float wheelBase_,
             const std::vector<Wheel>& wheels_, float driftDuration_, float driftSteeringMultiplier_,
             float hoodHeight_ = 10.0f, float roofHeight_ = 20.0f);

    // Control methods - moved from Car class
    void forward(float spd);
    void backward(float spd);
    void left(float spd);
    void right(float spd);
    
    // Interaction pointers - these should be initialized by derived classes
    Interaction* getInInter;
    Interaction* getOutInter;
    Interaction* prevRadioStationInter;
    Interaction* nextRadioStationInter;
    Interaction* fillTankInter;
	
    float steerInput;
	float throttleInput;
	float brakeInput;
};

} // namespace nyaa