#include "Map/New3DRenderer.h"
#include "Shader/New3DMainShader.h"
#include "Map/Map.h"
#include "Entity/Actor.h"
#include "Entity/ActorShadow.h"
#include "Texture/WalkAnim.h"
#include "Screen/InGameScreen.h"
#include <SFML/OpenGL.hpp>
#include <cmath>

namespace nyaa {

void New3DRenderer::renderActorShadows(Map* map, RendTarget& target)
{
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;
    
    // Setup OpenGL state for shadow rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE); // Shadows don't write to depth buffer
    
    // Collect and render shadows for all actors within distance
    for (auto& entity : map->getEntities()) {
        Actor* actor = dynamic_cast<Actor*>(entity);
        if (!actor) continue;
        
        // Skip if too far from camera
        if (!ActorShadow::isWithinShadowDistance(actor, camWorldX, camWorldZ)) continue;
        
        // Get actor position relative to camera
        float entityX = (float)(actor->posX / 16.0 - camWorldX);
        float entityZ = (float)(actor->posY / 16.0 - camWorldZ);
        
        // Get actor dimensions
        float width = actor->sizeX / 16.0f * actor->renderScaleX;
        float offsetX = -width * actor->originX;
        
        // Actor center for foot shadow
        float centerX = entityX + offsetX + width * 0.5f;
        float centerZ = entityZ;
        
        // --- Foot shadow: a simple transparent circle at the actor's feet ---
        // Shrinks as the actor jumps (posZ increases)
        {
            float jumpHeight = (float)(actor->posZ / 16.0); // posZ in pixels, convert to world
            
            // Base radius of the foot shadow circle
            float baseRadius = width * 0.35f;
            
            // Shrink radius as actor goes higher (min 20% of base at max jump)
            float radiusScale = 1.0f / (1.0f + jumpHeight * 2.0f);
            if (radiusScale < 0.2f) radiusScale = 0.2f;
            float radius = baseRadius * radiusScale;
            
            // Alpha fades as actor goes higher
            float baseAlpha = 0.4f;
            float alpha = baseAlpha * radiusScale;
            if (alpha < 0.05f) alpha = 0.05f;
            
            // Offset foot shadow based on height (use slight downward/forward direction)
            // This simulates sun coming from a typical angle
            float offsetX = jumpHeight * 0.5f;  // Slight forward offset
            float offsetZ = jumpHeight * 0.3f;  // Slight downward/toward camera
            
            float groundY = ActorShadow::GROUND_OFFSET;
            
            // Draw untextured circle using white texture
            glBindTexture(GL_TEXTURE_2D, whiteTexture);
            glColor4f(0.0f, 0.0f, 0.0f, alpha);
            
            const int segments = 16;
            glBegin(GL_TRIANGLE_FAN);
            // Center of circle (with height offset)
            glTexCoord2f(0.5f, 0.5f);
            glVertex3f(centerX + offsetX, groundY, centerZ + offsetZ);
            for (int s = 0; s <= segments; ++s) {
                float angle = s * 3.14159f * 2.0f / (float)segments;
                float cx = std::cos(angle) * radius;
                float cz = std::sin(angle) * radius * 0.5f; // Squish Z for perspective
                glTexCoord2f(0.5f + std::cos(angle) * 0.5f, 0.5f + std::sin(angle) * 0.5f);
                glVertex3f(centerX + offsetX + cx, groundY, centerZ + offsetZ + cz);
            }
            glEnd();
        }
        
        // --- Directional shadows from lights and sun ---
        ActorShadowData shadows[ActorShadow::MAX_SHADOWS_PER_ACTOR];
        int shadowCount = ActorShadow::calculateShadows(actor, map, shadows);
        
        if (shadowCount == 0) continue;
        
        float height = actor->sizeY / 16.0f * actor->renderScaleY;
        
        // Render each shadow
        for (int i = 0; i < shadowCount; ++i) {
            const ActorShadowData& shadow = shadows[i];
            
            // Get actor height in world units
            float actorHeight = (float)(actor->posZ / 16.0);
            
            // Shadow base position: offset from actor's feet based on height
            // Higher actors cast shadows further away (sun rays are parallel)
            float heightOffsetX = shadow.dirX * actorHeight * 0.8f;
            float heightOffsetZ = shadow.dirZ * actorHeight * 0.8f;
            
            float groundY = ActorShadow::GROUND_OFFSET;
            
            // Shadow stretches in the given direction
            float stretchX = shadow.dirX * shadow.length;
            float stretchZ = shadow.dirZ * shadow.length;
            
            // Build a projected quad on the ground plane:
            // The perpendicular direction determines the width of the shadow.
            float perpX = -shadow.dirZ;
            float perpZ =  shadow.dirX;
            float halfW = width * 0.5f;
            
            // Four corners of shadow quad on the ground
            // Near-left, Near-right (offset from actor's feet based on height)
            float x1 = centerX + heightOffsetX - perpX * halfW;
            float z1 = centerZ + heightOffsetZ - perpZ * halfW;
            float x2 = centerX + heightOffsetX + perpX * halfW;
            float z2 = centerZ + heightOffsetZ + perpZ * halfW;
            // Far-left, Far-right (shadow tip)
            float x3 = centerX + heightOffsetX + stretchX + perpX * halfW;
            float z3 = centerZ + heightOffsetZ + stretchZ + perpZ * halfW;
            float x4 = centerX + heightOffsetX + stretchX - perpX * halfW;
            float z4 = centerZ + heightOffsetZ + stretchZ - perpZ * halfW;
            
            // Use the sprite row and flip calculated based on shadow direction
            unsigned int row = shadow.spriteRow;
            bool flip = shadow.flipSprite;
            
            // Lambda to render a single WalkAnim layer as a shadow
            auto renderShadowLayer = [&](WalkAnim& anim) {
                if (anim.tex.getSize().x > 0 && anim.columns > 0 && anim.rows > 0) {
                    // Calculate texture coordinates for current frame
                    int frameWidth = anim.tex.getSize().x / anim.columns;
                    int frameHeight = anim.tex.getSize().y / anim.rows;
                    int frameX = anim.frame * frameWidth;
                    int frameY = row * frameHeight;
                    
                    float u1 = (float)frameX / (float)anim.tex.getSize().x;
                    float v1 = (float)frameY / (float)anim.tex.getSize().y;
                    float u2 = (float)(frameX + frameWidth) / (float)anim.tex.getSize().x;
                    float v2 = (float)(frameY + frameHeight) / (float)anim.tex.getSize().y;
                    
                    // Flip texture coordinates horizontally if needed
                    if (flip) {
                        float temp = u1;
                        u1 = u2;
                        u2 = temp;
                    }
                    
                    sf::Texture::bind(&anim.tex);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    
                    // Render shadow as black with alpha
                    glColor4f(0.0f, 0.0f, 0.0f, shadow.alpha);
                    
                    // Draw quad on ground plane
                    // Texture bottom (v2) = near edge (feet), texture top (v1) = far edge (tip)
                    glBegin(GL_QUADS);
                    glTexCoord2f(u1, v2); glVertex3f(x1, groundY, z1);
                    glTexCoord2f(u2, v2); glVertex3f(x2, groundY, z2);
                    glTexCoord2f(u2, v1); glVertex3f(x3, groundY, z3);
                    glTexCoord2f(u1, v1); glVertex3f(x4, groundY, z4);
                    glEnd();
                }
            };
            
            // Render all visible layers as shadow for complete silhouette
            // Order: body, eyes, hair, clothes, tail, outline
            renderShadowLayer(*actor->body);
            renderShadowLayer(*actor->eyes);
            renderShadowLayer(*actor->hair);
            renderShadowLayer(*actor->clothes);
            renderShadowLayer(*actor->tail);
            renderShadowLayer(*actor->outline);
        }
    }
    
    // Restore OpenGL state
    glDepthMask(GL_TRUE);
}

} // namespace nyaa
