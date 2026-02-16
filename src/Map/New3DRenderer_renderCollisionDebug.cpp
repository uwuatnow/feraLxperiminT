#include "Map/New3DRenderer.h"
#include "Map/Map.h"
#include "Screen/InGameScreen.h"
#include "Game/Mouse.h"
#include "Game/Util.h"
#include <SFML/OpenGL.hpp>
#include "Map/Building.h"

namespace nyaa {

void New3DRenderer::renderCollisionDebug(Map* map, RendTarget& target)
{
#if DEBUG
// Render collision lines if collision debug is enabled
    if (IGS->collisionDebug)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_TEXTURE_2D);
        
        double camWorldX = m_camera.getX() / 16.0;
        double camWorldZ = m_camera.getY() / 16.0;
        
        for (size_t i = 0; i < map->collLines.size(); ++i)
        {
            const auto& cl = map->collLines[i];
            float x1 = (float)((double)cl->p1.x / 16.0 - camWorldX);
            float z1 = (float)((double)cl->p1.y / 16.0 - camWorldZ);
            float x2 = (float)((double)cl->p2.x / 16.0 - camWorldX);
            float z2 = (float)((double)cl->p2.y / 16.0 - camWorldZ);
            
            // Wall face with transparency
            glColor4f(1.0f, 0.2f, 0.2f, 0.6f);
            glBegin(GL_QUADS);
            glVertex3f(x1, 0.0f, z1);
            glVertex3f(x2, 0.0f, z2);
            glVertex3f(x2, 2.0f, z2);
            glVertex3f(x1, 2.0f, z1);
            glEnd();
            
            // Bright outline
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            glLineWidth(3.0f);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x1, 0.0f, z1);
            glVertex3f(x2, 0.0f, z2);
            glVertex3f(x2, 2.0f, z2);
            glVertex3f(x1, 2.0f, z1);
            glEnd();
            
            // Render hover indicators for wall points in movingV2fps
            if (std::find(IGS->movingV2fps.begin(), IGS->movingV2fps.end(), &cl->p1) != IGS->movingV2fps.end())
            {
                glColor4f(0.0f, 1.0f, 0.0f, 0.8f);
                glLineWidth(5.0f);
                glBegin(GL_LINES);
                glVertex3f(x1, 0.0f, z1);
                glVertex3f(x1, 2.0f, z1);
                glEnd();
            }
            if (std::find(IGS->movingV2fps.begin(), IGS->movingV2fps.end(), &cl->p2) != IGS->movingV2fps.end())
            {
                glColor4f(0.0f, 1.0f, 0.0f, 0.8f);
                glLineWidth(5.0f);
                glBegin(GL_LINES);
                glVertex3f(x2, 0.0f, z2);
                glVertex3f(x2, 2.0f, z2);
                glEnd();
            }
        }
        
        glDisable(GL_BLEND);
    }
    if (IGS->buildingEditOn)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_TEXTURE_2D);
        
        double camWorldX = m_camera.getX() / 16.0;
        double camWorldZ = m_camera.getY() / 16.0;
        
        for (auto b : map->buildings)
        {
            float currentFloorHeight = 0.0f;
            for (auto fl : b->floors)
            {
                for (auto& w : fl->walls)
                {
                    float x1 = (float)((double)(w.x1 + b->posX) / 16.0 - camWorldX);
                    float z1 = (float)((double)(w.y1 + b->posY) / 16.0 - camWorldZ);
                    float x2 = (float)((double)(w.x2 + b->posX) / 16.0 - camWorldX);
                    float z2 = (float)((double)(w.y2 + b->posY) / 16.0 - camWorldZ);
                    
                    float h = fl->height / 16.0f;
                    float zBase = currentFloorHeight / 16.0f;
                    float zTop = zBase + h;

                    // Wall face
                    if (&w == IGS->grabbedBuildingWall) glColor4f(1.0f, 1.0f, 0.2f, 0.6f); // Yellow if grabbed
                    else if (&w == IGS->hoveredBuildingWall && IGS->hoveredBuildingWallAnchorIdx == 2) glColor4f(0.5f, 0.5f, 1.0f, 0.6f); // Lighter blue if hovered
                    else glColor4f(0.2f, 0.2f, 1.0f, 0.4f); // Blue otherwise
                    
                    glBegin(GL_QUADS);
                    glVertex3f(x1, zBase, z1);
                    glVertex3f(x2, zBase, z2);
                    glVertex3f(x2, zTop, z2);
                    glVertex3f(x1, zTop, z1);
                    glEnd();
                    
                    // Outlines
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                    glLineWidth(2.0f);
                    glBegin(GL_LINE_LOOP);
                    glVertex3f(x1, zBase, z1);
                    glVertex3f(x2, zBase, z2);
                    glVertex3f(x2, zTop, z2);
                    glVertex3f(x1, zTop, z1);
                    glEnd();

                    // Anchor indicators
                    glPointSize(10.0f);
                    glBegin(GL_POINTS);
                    
                    // Anchor 1
                    if ((&w == IGS->grabbedBuildingWall && IGS->grabbedBuildingWallAnchorIdx == 0) || 
                        (&w == IGS->hoveredBuildingWall && IGS->hoveredBuildingWallAnchorIdx == 0))
                        glColor4f(1.0f, 1.0f, 0.0f, 1.0f); // Yellow if hovered/grabbed
                    else glColor4f(0.0f, 1.0f, 0.0f, 1.0f); // Green otherwise
                    glVertex3f(x1, zBase, z1);

                    // Anchor 2
                    if ((&w == IGS->grabbedBuildingWall && IGS->grabbedBuildingWallAnchorIdx == 1) || 
                        (&w == IGS->hoveredBuildingWall && IGS->hoveredBuildingWallAnchorIdx == 1))
                        glColor4f(1.0f, 1.0f, 0.0f, 1.0f); // Yellow if hovered/grabbed
                    else glColor4f(0.0f, 1.0f, 0.0f, 1.0f); // Green otherwise
                    glVertex3f(x2, zBase, z2);
                    
                    glEnd();
                }
                currentFloorHeight += fl->height;
            }
        }
    }
#endif
}

}