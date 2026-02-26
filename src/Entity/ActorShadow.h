#pragma once
#include "Game/Globals.h"
#include <vector>
#include <algorithm>

namespace nyaa {

class Actor;
class Light;
class Map;

/**
 * @brief Represents a single shadow cast by a light source
 */
struct ActorShadowData
{
    float dirX;        // Shadow stretch direction X (normalized, away from light)
    float dirZ;        // Shadow stretch direction Z (normalized, away from light)
    float length;      // How long the shadow stretches
    float alpha;       // Shadow opacity (0.0-1.0)
    int spriteRow;     // Which row of WalkAnim to use (0=Up, 1=Down, 2=Left/Right)
    bool flipSprite;   // Whether to flip the sprite horizontally
    
    ActorShadowData() : dirX(0), dirZ(0), length(0), alpha(0), spriteRow(1), flipSprite(false) {}
};

/**
 * @brief Manages shadow calculations for actors
 * 
 * Calculates up to 4 shadows per actor based on nearby light sources and the sun.
 * Shadows are projected onto the ground plane and stretch away from lights.
 * On outdoor maps, a sun shadow is added whose direction depends on time of day.
 */
class ActorShadow
{
public:
    static constexpr float MAX_SHADOW_DISTANCE_TILES = 20.0f;
    static constexpr int MAX_SHADOWS_PER_ACTOR = 4;
    static constexpr float ACTOR_HEIGHT = 1.8f;  // Approximate actor height in world units
    static constexpr float GROUND_OFFSET = 0.08f; // Slightly above ground to avoid z-fighting and render over roads
    
    // Sun shadow constants
    static constexpr float SUN_SHADOW_MIN_LENGTH = 0.6f;   // Shortest shadow (noon)
    static constexpr float SUN_SHADOW_MAX_LENGTH = 3.0f;   // Longest shadow (sunrise/sunset)
    static constexpr float SUN_SHADOW_MAX_ALPHA = 0.55f;    // Darkest sun shadow (midday)
    static constexpr float SUN_SHADOW_MIN_ALPHA = 0.10f;    // Lightest sun shadow (near horizon)
    
public:
    /**
     * @brief Calculate shadows for an actor based on nearby lights and the sun
     * @param actor The actor to calculate shadows for
     * @param map The current map (to find lights and determine outdoor/indoor)
     * @param outShadows Output array to fill with shadow data (max MAX_SHADOWS_PER_ACTOR)
     * @return int Number of shadows calculated (0-MAX_SHADOWS_PER_ACTOR)
     */
    static int calculateShadows(Actor* actor, Map* map, ActorShadowData outShadows[MAX_SHADOWS_PER_ACTOR]);
    
    /**
     * @brief Check if an actor is within shadow rendering distance of camera
     * @param actor The actor to check
     * @param camWorldX Camera world X position
     * @param camWorldZ Camera world Z position
     * @return bool True if actor is close enough to render shadows
     */
    static bool isWithinShadowDistance(Actor* actor, double camWorldX, double camWorldZ);
    
private:
    /**
     * @brief Calculate influence of a light (higher = stronger shadow)
     * @param light The light entity
     * @param actorX Actor X position
     * @param actorY Actor Y position
     * @return float Influence value (0+), higher means stronger shadow
     */
    static float calculateLightInfluence(Light* light, double actorX, double actorY);
    
    /**
     * @brief Calculate which sprite row/direction to use for the shadow based on shadow angle
     * @param actor The actor being rendered
     * @param shadowDirX Shadow direction X (away from light source)
     * @param shadowDirZ Shadow direction Z (away from light source)
     * @param outRow Output: which row of WalkAnim to use (0=Up, 1=Down, 2=Left/Right)
     * @param outFlip Output: whether to flip the sprite horizontally
     */
    static void calculateSpriteDirection(Actor* actor, float shadowDirX, float shadowDirZ, int& outRow, bool& outFlip);
    
    /**
     * @brief Calculate a sun shadow for outdoor maps based on time of day
     * @param actor The actor to get a shadow for
     * @param timeOfDay Normalized time of day (0.0 = midnight, 0.5 = noon)
     * @param outShadow Output shadow data
     * @return bool True if a sun shadow should be visible (daytime hours)
     */
    static bool calculateSunShadow(Actor* actor, float timeOfDay, ActorShadowData& outShadow);
};

} // namespace nyaa
