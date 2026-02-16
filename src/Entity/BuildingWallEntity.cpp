#include "Entity/BuildingWallEntity.h"
#include "Game/Util.h"
#include "Game/Game.h"
#include "Game/Sfx.h"
#include "Map/Map.h"
#include "Entity/CarBase.h"
#include <cmath>

namespace nyaa {

bool disableBuildingWallPhysics = true;

BuildingWallEntity::BuildingWallEntity(double x1, double y1, double x2, double y2, float height, int textureId)
    : wallX1(x1)
    , wallY1(y1)
    , wallX2(x2)
    , wallY2(y2)
    , wallHeight(height)
    , texId(textureId)
    , health(INITIAL_HEALTH)
    , tiltAngle(0.0f)
    , tiltVelocity(0.0f)
    , knockedOver(false)
    , falling(false)
    , fallDirX(0.0f)
    , fallDirY(0.0f)
    , m_wallOBB(nullptr)
{
    // Calculate wall properties
    double dx = x2 - x1;
    double dy = y2 - y1;
    wallLength = (float)std::sqrt(dx * dx + dy * dy);
    wallAngle = (float)Util::ToDeg(std::atan2(dy, dx));
    
    // Position at center of wall
    posX = (x1 + x2) * 0.5;
    posY = (y1 + y2) * 0.5;
    posZ = 0.0f;
    
    // Set up OBB dimensions
    sizeX = wallLength;
    sizeY = WALL_THICKNESS;
    sizeZ = wallHeight;
    
    // Center origin
    originX = 0.5f;
    originY = 0.5f;
    
    // Wall direction
    dirAngle = wallAngle;
    
    // Physics properties - walls are heavy but can be pushed
    mass = 500.0f; // kg (heavy concrete/brick wall)
    inertia = 1000.0f; // High rotational inertia
    inertiaScale = 1.0f;
    friction = 0.8f;
    
    // Initialize OBB
    m_wallOBB = new OBB();
    m_wallOBB->center = { (float)posX, (float)posY };
    m_wallOBB->halfExtents = { wallLength * 0.5f, WALL_THICKNESS * 0.5f };
    m_wallOBB->angle = wallAngle;
    
    // Ensure minimum mass/inertia
    computeInertia();
}

BuildingWallEntity::~BuildingWallEntity()
{
    if (m_wallOBB) {
        delete m_wallOBB;
        m_wallOBB = nullptr;
    }
}

void BuildingWallEntity::update()
{
    if (!disableBuildingWallPhysics) {
        falling = false;
        knockedOver = false;
        tiltAngle = 0;
        tiltVelocity = 0;
        return;
    }
    
    // Update tilt physics if wall is falling
    if (falling && !knockedOver) {
        // Apply gravity to tilt velocity
        tiltVelocity += 9.81f * (G->frameDeltaMillis / 1000.0f);
        
        // Update tilt angle
        tiltAngle += tiltVelocity * (G->frameDeltaMillis / 1000.0f);
        
        // Apply friction to tilt velocity

        // Position calculation for Base Axis Pivot
        // The wall pivots around the line connecting (wallX1, wallY1) and (wallX2, wallY2).
        // The center of volume (posX, posY, posZ) moves.
        // Initially at height/2.
        // When tilted, the center moves perpendicular to the wall axis in the direction of fall.
        
        float tiltRad = Util::ToRad(tiltAngle);
        float offset = (wallHeight * 0.5f) * std::sin(tiltRad);
        
        double origCx = (wallX1 + wallX2) * 0.5;
        double origCy = (wallY1 + wallY2) * 0.5;
        
        posX = origCx + (double)(fallDirX * offset);
        posY = origCy + (double)(fallDirY * offset);
        posZ = (wallHeight * 0.5f) * std::cos(tiltRad) + (WALL_THICKNESS * 0.5f) * std::sin(tiltRad);

        // Clamp to ground
        if (tiltAngle >= MAX_TILT) {
            tiltAngle = MAX_TILT;
            tiltVelocity = 0.0f;
            knockedOver = true;
            falling = false;
            
            // Final position update
            offset = (wallHeight * 0.5f); // sin(90) = 1
            posX = origCx + (double)(fallDirX * offset);
            posY = origCy + (double)(fallDirY * offset);
            posZ = WALL_THICKNESS * 0.5f;

            // Play crash sound when wall hits ground
            Sfx::WallHit->play((float)posX, (float)posY);

            // Mark as "dead" so collision detection mostly ignores it
            // but keep it rendered as a fallen wall
            flags |= EntFlag_Dead;
        }
    }
    
    // Update OBB position
    if (m_wallOBB) {
        m_wallOBB->center = { (float)posX, (float)posY };
        m_wallOBB->angle = wallAngle;
    }
}

void BuildingWallEntity::hurt(float amount, DamageReason damageReason, Entity* damageSource)
{
    health -= amount;
    
    // If health drops below zero, start falling
    if (health <= 0.0f && !falling && !knockedOver) {
        falling = true;
        tiltVelocity = 20.0f; // Initial push
    }
    
    // If hurt by car collision, apply push force
    if (damageReason == DamageReason_VehicleCollision && damageSource) {
        if (auto car = dynamic_cast<CarBase*>(damageSource)) {
            // Calculate contact point (approximately)
            float carSpeed = 0.0f;
            float carVelX = 0.0f, carVelY = 0.0f;
            car->getVel(&carVelX, &carVelY);
            carSpeed = std::sqrt(carVelX * carVelX + carVelY * carVelY);
            
            // Push direction based on car velocity
            if (carSpeed > 1.0f) {
                float pushX = carVelX / carSpeed;
                float pushY = carVelY / carSpeed;
                
                // Find approximate contact point on wall
                float contactX = (float)posX;
                float contactY = (float)posY;
                
                // Calculate force magnitude based on car speed and mass
                float forceMag = carSpeed * car->mass * 0.1f;
                
                applyPushForce(pushX, pushY, forceMag, contactX, contactY);
            }
        }
    }
}

void BuildingWallEntity::playCollisionSfx()
{
    // Only play sound if we're not already knocked over
    if (!knockedOver) {
        Sfx::WallHit->play((float)posX, (float)posY, true);
    }
}

OBB* BuildingWallEntity::getOBB() const
{
    if (!m_wallOBB) {
        m_wallOBB = new OBB();
    }
    
    // Update OBB center to match current position
    m_wallOBB->center = { (float)posX, (float)posY };
    m_wallOBB->halfExtents = { wallLength * 0.5f, WALL_THICKNESS * 0.5f };
    
    // When wall is falling/tilted, the OBB rotates perpendicular to the wall direction
    // For Base Axis Pivot:
    // OBB remains aligned with the wall length.
    // Width (Thickness) becomes effectively height projection?
    // Actually, as it falls, the "top" view OBB widens because the wall is laying flat.
    // BUT OBB is 2D footprint.
    // If wall is flat on ground, OBB should cover the flat wall area.
    
    // Original Wall: Length x Thickness
    // Flat Wall: Length x Height
    
    // Determine projected size on ground
    float tiltRad = Util::ToRad(tiltAngle);
    float projectedHeight = wallHeight * std::sin(tiltRad) + WALL_THICKNESS * std::cos(tiltRad);
    // Actually, simpler approximation:
    // When upright: Thickness
    // When flat: Height
    
    float currentThickness = WALL_THICKNESS + (wallHeight - WALL_THICKNESS) * (tiltAngle / 90.0f);
    
    m_wallOBB->halfExtents = { wallLength * 0.5f, currentThickness * 0.5f };
    m_wallOBB->angle = wallAngle; // Angle stays same! We just get wider/move center.
    
    // Center is updated in update() so it's handled.
    
    return m_wallOBB;
}

void BuildingWallEntity::applyPushForce(float forceX, float forceY, float forceMagnitude, float contactX, float contactY)
{
    if (knockedOver || falling) return;

    // Check if force is above threshold
    if (forceMagnitude < PUSH_FORCE_THRESHOLD) {
        // Just apply damage without knocking over
        float damage = forceMagnitude * 0.005f;
        if (damage > 1.0f) {
            hurt(damage, DamageReason_VehicleCollision, nullptr);
        }
        return;
    }

    // Determine fall direction based on push
    // We want the wall to fall AWAY from the push.
    // Calculate wall normal
    float dx = (float)(wallX2 - wallX1);
    float dy = (float)(wallY2 - wallY1);
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.0001f) return;
    float wallNormalX = -dy / len;
    float wallNormalY = dx / len;
    
    // Project push force onto normal
    float dot = forceX * wallNormalX + forceY * wallNormalY;
    
    // Set fall direction to the normal component of the push
    if (dot > 0) {
        fallDirX = wallNormalX;
        fallDirY = wallNormalY;
    } else {
        fallDirX = -wallNormalX;
        fallDirY = -wallNormalY;
    }

    // Calculate impulse needed to start the wall falling
    float requiredImpulse = PUSH_FORCE_THRESHOLD * 0.5f;
    float actualImpulse = std::min(forceMagnitude * 0.1f, requiredImpulse * 2.0f);

    // Add initial tilt velocity
    tiltVelocity = actualImpulse / mass * 20.0f; // Scale factor for gameplay feel

    // Start falling
    falling = true;

    // Play initial hit sound
    Sfx::WallHit->play((float)posX, (float)posY, true);

    // Apply damage
    float damage = forceMagnitude * 0.01f;
    health -= damage;
}

} // namespace nyaa
