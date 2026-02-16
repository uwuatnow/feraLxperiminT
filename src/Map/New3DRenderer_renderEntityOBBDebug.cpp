#include "Map/New3DRenderer.h"
#include "Map/Map.h"
#include "Entity/Actor.h"
#include "Game/Util.h"
#include "Game/OBB.h"
#include <SFML/OpenGL.hpp>

namespace nyaa {

void New3DRenderer::renderEntityOBBDebug(Map* map, RendTarget& target)
{
#if DEBUG
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    auto entities = map->getEntities();
    for (Entity* ent : entities)
    {
        if (ent->flags & Entity::EntFlag_UnderAllOtherEnts) continue;

        OBB* obb = ent->getOBB();
        Point corners[4];
        obb->getCorners(corners);

        // Check if colliding with any other actor
        bool colliding = false;
        for (Entity* other : entities)
        {
            if (other == ent) continue;

            OBB* otherObb = other->getOBB();
            if (OBBIntersects(*obb, *otherObb))
            {
                colliding = true;
                break;
            }
        }

        // Set color: red if colliding, green if not
        float r = colliding ? 1.0f : 0.0f;
        float g = colliding ? 0.0f : 1.0f;
        float b = 0.0f;
        float a = 0.5f;

        // Draw filled quad
        glColor4f(r, g, b, a);
        glBegin(GL_QUADS);
        for (int i = 0; i < 4; ++i)
        {
            float x = (float)((double)corners[i].x / 16.0 - camWorldX);
            float z = (float)((double)corners[i].y / 16.0 - camWorldZ);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(x, 0.01f, z);
        }
        glEnd();

        // Draw outline
        glColor4f(r, g, b, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 4; ++i)
        {
            float x = (float)((double)corners[i].x / 16.0 - camWorldX);
            float z = (float)((double)corners[i].y / 16.0 - camWorldZ);
            glVertex3f(x, 0.01f, z);
        }
        glEnd();
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
#endif
}

}