#include "Map/New3DRenderer.h"
#include "Map/Map.h"
#include "Screen/InGameScreen.h"
#include "Entity/Entity.h"
#include <SFML/OpenGL.hpp>
#include <cmath>

namespace nyaa {

void New3DRenderer::renderVelocityDebug(Map* map, RendTarget& target)
{
#if DEBUG
    if (IGS->velocityDebug)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        
        double camWorldX = m_camera.getX() / 16.0;
        double camWorldZ = m_camera.getY() / 16.0;

        for (Entity* entity : map->getEntities()) {
            sf::Vector2f vel{};
            entity->getVel(&vel.x, &vel.y);
            double velX_px = (double)vel.x;
            double velY_px = (double)vel.y;
            double velMagnitude = sqrt(velX_px * velX_px + velY_px * velY_px);
            
            if (velMagnitude > 0.01) // Only show if actually moving
            {
                float relEntityX = (float)(entity->posX / 16.0 - camWorldX);
                float relEntityZ = (float)(entity->posY / 16.0 - camWorldZ);
                float entityY = 0.1f; // Just above ground
                
                // Scale velocity for visibility (much larger scale)
                float scale = 2.0f; // Base scale
                float velX_unit = (float)(velX_px / velMagnitude) * scale;
                float velZ_unit = (float)(velY_px / velMagnitude) * scale;
                
                // Color based on velocity magnitude (green = slow, red = fast)
                float normalizedSpeed = std::min((float)velMagnitude / 50.0f, 1.0f);
                glColor4f(normalizedSpeed, 1.0f - normalizedSpeed, 0.0f, 1.0f);
                
                // Draw velocity arrow
                glLineWidth(4.0f);
                glBegin(GL_LINES);
                glVertex3f(relEntityX, entityY, relEntityZ);
                glVertex3f(relEntityX + velX_unit, entityY, relEntityZ + velZ_unit);
                glEnd();
                
                // Draw arrowhead
                float arrowSize = scale * 0.2f;
                float angle = atan2(velZ_unit, velX_unit);
                float arrowAngle1 = angle + 2.618f; // 150 degrees
                float arrowAngle2 = angle - 2.618f; // -150 degrees
                
                glBegin(GL_LINES);
                glVertex3f(relEntityX + velX_unit, entityY, relEntityZ + velZ_unit);
                glVertex3f(relEntityX + velX_unit + cos(arrowAngle1) * arrowSize, entityY, relEntityZ + velZ_unit + sin(arrowAngle1) * arrowSize);
                glVertex3f(relEntityX + velX_unit, entityY, relEntityZ + velZ_unit);
                glVertex3f(relEntityX + velX_unit + cos(arrowAngle2) * arrowSize, entityY, relEntityZ + velZ_unit + sin(arrowAngle2) * arrowSize);
                glEnd();
            }
        }
        
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
#endif
}

}