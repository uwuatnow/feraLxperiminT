#include "Map/New3DRenderer.h"
#include "Map/Map.h"
#include <SFML/OpenGL.hpp>
#include "Game/Game.h"
#include <iostream>

namespace nyaa {

void New3DRenderer::renderPortals(Map* map)
{
#if DEBUG
    if (map->portals.empty()) return;

    // Use white texture for untextured geometry (required by shader)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    glNormal3f(0.0f, 1.0f, 0.0f); // Face up

    for (const auto& portal : map->portals)
    {
        float portalX = (float)((double)portal.tilePosX - camWorldX);
        float portalZ = (float)((double)portal.tilePosY - camWorldZ);
        
        // Blue portal base
        glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(portalX, 0.02f, portalZ);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(portalX + 1.0f, 0.02f, portalZ);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(portalX + 1.0f, 0.02f, portalZ + 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(portalX, 0.02f, portalZ + 1.0f);
        glEnd();
        
        // White inner border
        float inset = 0.0625f;
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(portalX + inset, 0.025f, portalZ + inset);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(portalX + 1.0f - inset, 0.025f, portalZ + inset);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(portalX + 1.0f - inset, 0.025f, portalZ + 1.0f - inset);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(portalX + inset, 0.025f, portalZ + 1.0f - inset);
        glEnd();
        
        // Direction indicator line
        float centerX = portalX + 0.5f;
        float centerZ = portalZ + 0.5f;
        float angle = (portal.outDirAngleDeg - 180) * 3.14159f / 180.0f;
        float lineLength = 0.625f;
        float endX = centerX + cos(angle) * lineLength;
        float endZ = centerZ + sin(angle) * lineLength;
        
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glTexCoord2f(0.5f, 0.5f); glVertex3f(centerX, 0.03f, centerZ);
        glTexCoord2f(0.5f, 0.5f); glVertex3f(endX, 0.03f, endZ);
        glEnd();
    }
    
    glDisable(GL_BLEND);
#endif
}

}
