#include "Map/New3DRenderer.h"
#include "Screen/InGameScreen.h"
#include "Game/Game.h"
#include "Texture/Sheet.h"
#include "Shader/New3DMainShader.h"
#include <SFML/OpenGL.hpp>

namespace nyaa {

void New3DRenderer::renderParticles3D()
{
    if (!IGS) return;
    
    ParticleSystem& ps = IGS->particleSystem;
    int particleCount = ps.getParticleCount();
    if (particleCount == 0) return;

    if (m_new3DMainShader) {
        m_new3DMainShader->setFullbright(true);
        m_new3DMainShader->update();
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Get camera position for relative positioning
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    // Get current modelview matrix to extract camera vectors
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    // Extract right and up vectors from camera matrix
    float rightX = modelview[0], rightY = modelview[4], rightZ = modelview[8];
    float upX = modelview[1], upY = modelview[5], upZ = modelview[9];

    const Particle* particles = ps.getParticles();

    for (int i = 0; i < ParticleSystem::MaxParticles; ++i)
    {
        const Particle& p = particles[i];
        if (!p.active) continue;

        // Convert world position to camera-relative coordinates (like roads do)
        float worldX = (float)(p.x / 16.0 - camWorldX);
        float worldY = (float)(p.z / 16.0);  // z is height
        float worldZ = (float)(p.y / 16.0 - camWorldZ);  // y is depth in the game

        float halfSize = p.size * 0.5f;

        // Create billboard quad using camera vectors
        float x1 = worldX - rightX * halfSize - upX * halfSize;
        float y1 = worldY - rightY * halfSize - upY * halfSize;
        float z1 = worldZ - rightZ * halfSize - upZ * halfSize;
        
        float x2 = worldX + rightX * halfSize - upX * halfSize;
        float y2 = worldY + rightY * halfSize - upY * halfSize;
        float z2 = worldZ + rightZ * halfSize - upZ * halfSize;
        
        float x3 = worldX + rightX * halfSize + upX * halfSize;
        float y3 = worldY + rightY * halfSize + upY * halfSize;
        float z3 = worldZ + rightZ * halfSize + upZ * halfSize;
        
        float x4 = worldX - rightX * halfSize + upX * halfSize;
        float y4 = worldY - rightY * halfSize + upY * halfSize;
        float z4 = worldZ - rightZ * halfSize + upZ * halfSize;

        // Set color with alpha (use glColor4ub like roads do)
        unsigned char alpha = (unsigned char)(p.a * 255.0f);
        glColor4ub((unsigned char)(p.r * 255.0f),
                   (unsigned char)(p.g * 255.0f),
                   (unsigned char)(p.b * 255.0f),
                   alpha);

        // Draw billboard quad
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x1, y1, z1);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x2, y2, z2);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x3, y3, z3);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x4, y4, z4);
        glEnd();
    }

    if (m_new3DMainShader) {
        m_new3DMainShader->setFullbright(false);
        m_new3DMainShader->update();
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

} // namespace nyaa