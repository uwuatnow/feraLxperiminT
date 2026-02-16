#include "Entity/ActorShadow.h"
#include "Entity/Actor.h"
#include "Entity/Light.h"
#include "Map/Map.h"
#include "Screen/InGameScreen.h"
#include "Game/GameClock.h"
#include <cmath>

namespace nyaa {

int ActorShadow::calculateShadows(Actor* actor, Map* map, ActorShadowData outShadows[MAX_SHADOWS_PER_ACTOR])
{
    if (!actor || !map) return 0;
    
    double actorX = actor->posX / 16.0;
    double actorY = actor->posY / 16.0;
    
    int shadowCount = 0;
    
    // --- Sun shadow for outdoor maps ---
    if (map->type == Map::Type_Outdoors && IGS && IGS->gameClock) {
        float timeOfDay = IGS->gameClock->getTimeOfDay();
        ActorShadowData sunShadow;
        if (calculateSunShadow(actor, timeOfDay, sunShadow)) {
            outShadows[shadowCount] = sunShadow;
            shadowCount++;
        }
    }
    
    // --- Point-light shadows ---
    // Collect all lights with their influence
    struct LightInfluence {
        Light* light;
        float influence;
        float dirX;  // from light to actor (shadow direction)
        float dirZ;
        float distance;
    };
    
    std::vector<LightInfluence> lights;
    
    for (auto& entity : map->getEntities()) {
        Light* light = dynamic_cast<Light*>(entity);
        if (!light) continue;
        
        float influence = calculateLightInfluence(light, actorX, actorY);
        if (influence <= 0.0f) continue;
        
        double lightX = light->posX / 16.0;
        double lightY = light->posY / 16.0;
        
        // Direction from light TO actor (shadow stretches away from light)
        float dx = (float)(actorX - lightX);
        float dz = (float)(actorY - lightY);
        float dist = std::sqrt(dx * dx + dz * dz);
        
        if (dist > 0.001f) {
            dx /= dist;
            dz /= dist;
        }
        
        lights.push_back({light, influence, dx, dz, dist});
    }
    
    // Sort by influence (descending)
    std::sort(lights.begin(), lights.end(), [](const LightInfluence& a, const LightInfluence& b) {
        return a.influence > b.influence;
    });
    
    // Take remaining slots
    for (size_t i = 0; i < lights.size() && shadowCount < MAX_SHADOWS_PER_ACTOR; ++i) {
        const auto& li = lights[i];
        
        // Shadow direction is FROM the light source (actor to light direction already computed)
        outShadows[shadowCount].dirX = li.dirX;
        outShadows[shadowCount].dirZ = li.dirZ;
        
        // Shadow length based on actor height and light distance
        // Closer lights = shorter shadows, farther lights = longer shadows
        float heightFactor = ACTOR_HEIGHT;
        float distanceFactor = 1.0f + (li.distance / 5.0f);
        outShadows[shadowCount].length = heightFactor * distanceFactor * 0.5f;
        
        // Alpha based on light intensity and distance
        float intensity = li.light->intensity / 255.0f;
        float distanceAttenuation = 1.0f / (1.0f + li.distance * 0.1f);
        outShadows[shadowCount].alpha = intensity * distanceAttenuation * 0.8f;
        
        // Clamp alpha
        if (outShadows[shadowCount].alpha > 0.8f) outShadows[shadowCount].alpha = 0.8f;
        if (outShadows[shadowCount].alpha < 0.1f) outShadows[shadowCount].alpha = 0.1f;
        
        // Calculate which sprite direction to use based on shadow direction
        // Pass shadow direction (away from light) so sprite faces correctly
        calculateSpriteDirection(actor, outShadows[shadowCount].dirX, outShadows[shadowCount].dirZ, 
                                outShadows[shadowCount].spriteRow, 
                                outShadows[shadowCount].flipSprite);
        
        shadowCount++;
    }
    
    return shadowCount;
}

bool ActorShadow::isWithinShadowDistance(Actor* actor, double camWorldX, double camWorldZ)
{
    if (!actor) return false;
    
    double actorX = actor->posX / 16.0;
    double actorY = actor->posY / 16.0;
    
    double dx = actorX - camWorldX;
    double dy = actorY - camWorldZ;
    double dist = std::sqrt(dx * dx + dy * dy);
    
    return dist <= MAX_SHADOW_DISTANCE_TILES;
}

float ActorShadow::calculateLightInfluence(Light* light, double actorX, double actorY)
{
    if (!light) return 0.0f;
    
    double lightX = light->posX / 16.0;
    double lightY = light->posY / 16.0;
    
    double dx = actorX - lightX;
    double dy = actorY - lightY;
    double dist = std::sqrt(dx * dx + dy * dy);
    
    // Lights beyond 10 tiles have minimal influence
    if (dist > 10.0) return 0.0f;
    
    float intensity = light->intensity / 255.0f;
    
    // Influence = intensity / distance^2
    float influence = intensity / (1.0f + static_cast<float>(dist * dist) * 0.01f);
    
    return influence;
}

void ActorShadow::calculateSpriteDirection(Actor* actor, float shadowDirX, float shadowDirZ, int& outRow, bool& outFlip)
{
    // The shadow stretches in the direction (shadowDirX, shadowDirZ) away from the light.
    // We pick the sprite row that best represents the silhouette as seen from the light's direction.
    // The light is in the OPPOSITE direction of the shadow, so the silhouette we see
    // is the actor viewed from the direction (-shadowDirX, -shadowDirZ).
    // 
    // Convention: -Z = up on screen (row 0 = Up/back), +Z = down on screen (row 1 = Down/front)
    //             -X = left (row 2, no flip), +X = right (row 2, flip)
    //
    // When light comes from -Z (above), shadow goes +Z (down), we see actor's back -> row 0 (Up)
    // When light comes from +Z (below), shadow goes -Z (up),   we see actor's front -> row 1 (Down)
    
    // lightDir = direction FROM actor TO light = opposite of shadow direction
    float lightDirX = -shadowDirX;
    float lightDirZ = -shadowDirZ;
    
    float absX = std::abs(lightDirX);
    float absZ = std::abs(lightDirZ);
    
    if (absZ >= absX) {
        // Light is primarily in Z direction
        if (lightDirZ < 0) {
            // Light is above/behind actor (negative Z), shadow stretches down (+Z)
            // We see the actor's back
            outRow = 0; // Up row (back view)
            outFlip = false;
        } else {
            // Light is below/in-front of actor (positive Z), shadow stretches up (-Z)
            // We see the actor's front
            outRow = 1; // Down row (front view)
            outFlip = false;
        }
    } else {
        // Light is primarily in X direction
        outRow = 2; // Left/Right row
        if (lightDirX > 0) {
            // Light is to the right, shadow stretches left
            outFlip = true;
        } else {
            // Light is to the left, shadow stretches right
            outFlip = false;
        }
    }
}

bool ActorShadow::calculateSunShadow(Actor* actor, float timeOfDay, ActorShadowData& outShadow)
{
    // timeOfDay is 0.0 = midnight, 0.5 = noon, 1.0 = midnight again
    // Sun is only visible during roughly 6:00 (0.25) to 20:00 (0.833)
    
    const float sunriseTime = 6.0f / 24.0f;   // 0.25
    const float sunsetTime  = 20.0f / 24.0f;   // 0.833
    const float noonTime    = 12.0f / 24.0f;   // 0.5
    
    // No shadow at night
    if (timeOfDay < sunriseTime || timeOfDay > sunsetTime) return false;
    
    // Transition zones: fade in during first hour of sunrise, fade out during last hour before sunset
    const float transitionDuration = 1.0f / 24.0f; // 1 hour
    float sunStrength = 1.0f;
    if (timeOfDay < sunriseTime + transitionDuration) {
        sunStrength = (timeOfDay - sunriseTime) / transitionDuration;
    } else if (timeOfDay > sunsetTime - transitionDuration) {
        sunStrength = (sunsetTime - timeOfDay) / transitionDuration;
    }
    if (sunStrength <= 0.0f) return false;
    
    // Sun angle: maps time of day to an angle
    // At sunrise (0.25), sun is in the east -> shadow stretches west (-X, slight +Z)
    // At noon (0.5), sun is overhead -> very short shadow, stretches slightly +Z
    // At sunset (0.833), sun is in the west -> shadow stretches east (+X, slight +Z)
    
    // Normalize time within daylight range to [0, 1] where 0 = sunrise, 1 = sunset
    float dayProgress = (timeOfDay - sunriseTime) / (sunsetTime - sunriseTime);
    
    // Sun azimuth angle: sweeps from east to west
    // At dayProgress=0 (sunrise): sun is in the east, shadow points west (-X direction)
    // At dayProgress=0.5 (noon): sun is south, shadow points north (-Z direction, short)
    // At dayProgress=1 (sunset): sun is in the west, shadow points east (+X direction)
    // We'll use a half-circle sweep: angle goes from PI to 0
    float sunAzimuth = 3.14159f * (1.0f - dayProgress);  // PI -> 0
    
    // Shadow direction is opposite the sun direction. 
    // Sun direction vector (pointing FROM actor TO sun):
    //   sunDirX = cos(sunAzimuth)   (east at 0, west at PI)
    //   sunDirZ = -|sin(sunAzimuth)| (sun is always "above" in Z sense, shadows go +Z / toward viewer)
    // Shadow = opposite = away from sun
    float shadowDirX = -std::cos(sunAzimuth);
    float shadowDirZ = std::abs(std::sin(sunAzimuth)) * 0.3f + 0.1f; // slight downward bias (+Z = toward camera)
    
    // Normalize
    float len = std::sqrt(shadowDirX * shadowDirX + shadowDirZ * shadowDirZ);
    if (len > 0.001f) {
        shadowDirX /= len;
        shadowDirZ /= len;
    }
    
    outShadow.dirX = shadowDirX;
    outShadow.dirZ = shadowDirZ;
    
    // Sun elevation: highest at noon, lowest at sunrise/sunset
    // elevation goes from 0 (horizon) at sunrise/sunset to 1 (zenith) at noon
    float elevation = std::sin(dayProgress * 3.14159f); // 0 at edges, 1 at center
    
    // Shadow length: short when sun is high, long when sun is low
    outShadow.length = SUN_SHADOW_MIN_LENGTH + (SUN_SHADOW_MAX_LENGTH - SUN_SHADOW_MIN_LENGTH) * (1.0f - elevation);
    
    // Shadow alpha: darker at midday (strong sun), lighter near sunrise/sunset
    outShadow.alpha = SUN_SHADOW_MIN_ALPHA + (SUN_SHADOW_MAX_ALPHA - SUN_SHADOW_MIN_ALPHA) * elevation;
    outShadow.alpha *= sunStrength; // Fade during transition
    
    if (outShadow.alpha < 0.05f) return false;
    
    // Calculate sprite direction
    calculateSpriteDirection(actor, outShadow.dirX, outShadow.dirZ, outShadow.spriteRow, outShadow.flipSprite);
    
    return true;
}

} // namespace nyaa
