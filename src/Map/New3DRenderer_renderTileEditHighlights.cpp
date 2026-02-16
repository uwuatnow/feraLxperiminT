#include "Map/New3DRenderer.h"
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include <SFML/OpenGL.hpp>

namespace nyaa {

void New3DRenderer::renderTileEditHighlights(Map* map, RendTarget& target)
{
#if DEBUG
    // Render 3D tile editing highlights
    if (IGS->tileEditOn)
    {
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        double camWorldX = m_camera.getX() / 16.0;
        double camWorldZ = m_camera.getY() / 16.0;
        
        // Mouse tile highlight (yellow)
        float mouseTileX = (float)((double)IGS->mouseTilePosX - camWorldX);
        float mouseTileZ = (float)((double)IGS->mouseTilePosY - camWorldZ);
        glColor4f(1.0f, 1.0f, 0.2f, 0.6f);
        
        if (IGS->selectedLayer == 1) {
            // For obstacle layer, draw highlight on vertical face like obstacle tiles
            glBegin(GL_QUADS);
            glVertex3f(mouseTileX, 0, mouseTileZ + 1.0f);
            glVertex3f(mouseTileX + 1.0f, 0, mouseTileZ + 1.0f);
            glVertex3f(mouseTileX + 1.0f, 1.0f, mouseTileZ + 1.0f);
            glVertex3f(mouseTileX, 1.0f, mouseTileZ + 1.0f);
            glEnd();
        } else if (IGS->selectedLayer == 2) {
            // For ceiling layer, draw highlight at ceiling height
            glBegin(GL_QUADS);
            glVertex3f(mouseTileX, 2.01f, mouseTileZ);
            glVertex3f(mouseTileX + 1.0f, 2.01f, mouseTileZ);
            glVertex3f(mouseTileX + 1.0f, 2.01f, mouseTileZ + 1.0f);
            glVertex3f(mouseTileX, 2.01f, mouseTileZ + 1.0f);
            glEnd();
            // Add shadow on floor
            glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
            glBegin(GL_QUADS);
            glVertex3f(mouseTileX, 0.005f, mouseTileZ);
            glVertex3f(mouseTileX + 1.0f, 0.005f, mouseTileZ);
            glVertex3f(mouseTileX + 1.0f, 0.005f, mouseTileZ + 1.0f);
            glVertex3f(mouseTileX, 0.005f, mouseTileZ + 1.0f);
            glEnd();
        } else {
            // For floor layer, draw highlight on floor
            glBegin(GL_QUADS);
            glVertex3f(mouseTileX, 0.01f, mouseTileZ);
            glVertex3f(mouseTileX + 1.0f, 0.01f, mouseTileZ);
            glVertex3f(mouseTileX + 1.0f, 0.01f, mouseTileZ + 1.0f);
            glVertex3f(mouseTileX, 0.01f, mouseTileZ + 1.0f);
            glEnd();
        }
        
        // Mouse chunk highlight (white)
        float mouseChunkX = (float)((double)IGS->mouseChunkPosX * 16.0 - camWorldX);
        float mouseChunkZ = (float)((double)IGS->mouseChunkPosY * 16.0 - camWorldZ);
        glColor4f(1.0f, 1.0f, 1.0f, 0.15f);
        glBegin(GL_QUADS);
        glVertex3f(mouseChunkX, 0.005f, mouseChunkZ);
        glVertex3f(mouseChunkX + 16.0f, 0.005f, mouseChunkZ);
        glVertex3f(mouseChunkX + 16.0f, 0.005f, mouseChunkZ + 16.0f);
        glVertex3f(mouseChunkX, 0.005f, mouseChunkZ + 16.0f);
        glEnd();
        
        // Player front tile highlight (red)
        auto front = sf::Vector2i{};
        IGS->player->getTileInFront(Direction_Default, &front.x, &front.y);
        float frontTileX = (float)((double)front.x - camWorldX);
        float frontTileZ = (float)((double)front.y - camWorldZ);
        glColor4f(1.0f, 0.0f, 0.0f, 0.6f);
        
        if (IGS->selectedLayer == 1) {
            // For obstacle layer, draw highlight on vertical face like obstacle tiles
            glBegin(GL_QUADS);
            glVertex3f(frontTileX, 0, frontTileZ + 1.0f);
            glVertex3f(frontTileX + 1.0f, 0, frontTileZ + 1.0f);
            glVertex3f(frontTileX + 1.0f, 1.0f, frontTileZ + 1.0f);
            glVertex3f(frontTileX, 1.0f, frontTileZ + 1.0f);
            glEnd();
        } else if (IGS->selectedLayer == 2) {
            // For ceiling layer, draw highlight at ceiling height
            glBegin(GL_QUADS);
            glVertex3f(frontTileX, 2.015f, frontTileZ);
            glVertex3f(frontTileX + 1.0f, 2.015f, frontTileZ);
            glVertex3f(frontTileX + 1.0f, 2.015f, frontTileZ + 1.0f);
            glVertex3f(frontTileX, 2.015f, frontTileZ + 1.0f);
            glEnd();
            // Add shadow on floor
            glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
            glBegin(GL_QUADS);
            glVertex3f(frontTileX, 0.01f, frontTileZ);
            glVertex3f(frontTileX + 1.0f, 0.01f, frontTileZ);
            glVertex3f(frontTileX + 1.0f, 0.01f, frontTileZ + 1.0f);
            glVertex3f(frontTileX, 0.01f, frontTileZ + 1.0f);
            glEnd();
        } else {
            // For floor layer, draw highlight on floor
            glBegin(GL_QUADS);
            glVertex3f(frontTileX, 0.015f, frontTileZ);
            glVertex3f(frontTileX + 1.0f, 0.015f, frontTileZ);
            glVertex3f(frontTileX + 1.0f, 0.015f, frontTileZ + 1.0f);
            glVertex3f(frontTileX, 0.015f, frontTileZ + 1.0f);
            glEnd();
        }
        
        glDisable(GL_BLEND);
    }
#endif
}

}