#include "Map/New3DRenderer.h"
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include "Game/Game.h"
#include "Map/Pathfinding.h"
#include <SFML/OpenGL.hpp>
#include <cmath>

namespace nyaa {

void New3DRenderer::renderAutoWalkParticles()
{
    // Update and render auto-walk destination particles
    float fadeSpeed = 3.0f;
    if (IGS->playerAutoWalking) {
        particleOpacity = std::min(1.0f, particleOpacity + fadeSpeed * G->frameDeltaMillis * 0.001f);
    } else {
        particleOpacity = std::max(0.0f, particleOpacity - fadeSpeed * G->frameDeltaMillis * 0.001f);
    }
    
    if (particleOpacity <= 0.0f) return;

    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    double destX = (double)IGS->playerAutoWalkToPosX / 16.0;
    double destZ = (double)IGS->playerAutoWalkToPosY / 16.0;
    
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Animate particles with time
    float time = particleTimer.getElapsedSeconds();
    
    // Draw path particles
    if (IGS->player->ast && IGS->player->ast->valid()) {
        PathfindNode* node = IGS->player->ast->startNode;
        int particleIndex = 0;
        while (node && node->child) {
            sf::Vector2f start = { (float)node->posX, (float)node->posY };
            sf::Vector2f end = { (float)node->child->posX, (float)node->child->posY };
            
            // Draw particles along the path segment
            float segmentLength = sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));
            int numParticles = (int)(segmentLength / 8.0f) + 1; // 1 particle per 8 pixels
            for (int i = 0; i < numParticles; i++) {
                float t = i / (float)(numParticles - 1);
                float relX = (float)((double)(start.x + (end.x - start.x) * t) / 16.0 - camWorldX);
                float relZ = (float)((double)(start.y + (end.y - start.y) * t) / 16.0 - camWorldZ);
                float y = 0.1f + sin(time * 2.0f + (float)particleIndex * 0.5f) * 0.05f;
                
                glColor4f(1.0f, 1.0f, 1.0f, 0.6f * particleOpacity);
                glPointSize(3.0f);
                glBegin(GL_POINTS);
                glVertex3f(relX, y, relZ);
                glEnd();
                particleIndex++;
            }
            node = node->child;
        }
    }
    
    // Draw 8 particles in a circle above the destination
    for (int i = 0; i < 8; i++) {
        float angle = (i / 8.0f) * 2.0f * 3.14159f + time;
        float radius = 0.3f;
        float relX = (float)(destX + (double)cos(angle) * radius - camWorldX);
        float relZ = (float)(destZ + (double)sin(angle) * radius - camWorldZ);
        float y = 0.5f + (float)sin((double)time * 3.0 + (double)i) * 0.2f;
        
        glColor4f(0.0f, 1.0f, 0.0f, 0.8f * particleOpacity);
        glPointSize(6.0f);
        glBegin(GL_POINTS);
        glVertex3f(relX, y, relZ);
        glEnd();
    }
    
    glDisable(GL_BLEND);
}

}