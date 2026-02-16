#include "Map/New3DRenderer.h"
#include "Map/Map.h"
#include "Entity/UrinePuddle.h"
#include <SFML/OpenGL.hpp>
#include "Game/Timer.h"
#include "Game/Util.h"

namespace nyaa {

void New3DRenderer::renderUrinePuddles(Map* map) const
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;
    
    glDepthMask(GL_FALSE);
    
    for (auto& entity : map->entities) {
        if (UrinePuddle* puddle = dynamic_cast<UrinePuddle*>(entity)) {
            float puddleX = (float)((double)puddle->posX / 16.0 - camWorldX);
            float puddleZ = (float)((double)puddle->posY / 16.0 - camWorldZ);
            
            float time = Util::Clamp(puddle->puddleSpreadTimer->secs(), 0, puddle->seconds);
            float size = Util::ScaleClamped(time, 0, puddle->seconds, 0.1f, puddle->PuddleRadiusPx);
            float radius = size / 16.f;
            
            // Yellow puddle color with transparency
            glColor4f(1.0f, 1.0f, 0.0f, 0.6f);
            
            // Draw puddle as circle on floor
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(puddleX, 0.01f, puddleZ); // Center
            for (int i = 0; i <= 16; i++) {
                float angle = i * 2.0f * 3.14159f / 16.0f;
                float x = puddleX + cos(angle) * radius;
                float z = puddleZ + sin(angle) * radius;
                glVertex3f(x, 0.01f, z);
            }
            glEnd();
        }
    }
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

}