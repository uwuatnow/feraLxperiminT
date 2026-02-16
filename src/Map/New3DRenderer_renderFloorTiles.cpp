#include "Map/New3DRenderer.h"
#include "Map/Map.h"
#include "Map/MapChunk.h"
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>
#include <GL/glu.h>
#include "Texture/Sheet.h"
#include "Entity/Actor.h"
#include "Screen/InGameScreen.h"
#include "Shader/SphericalAberrationShader.h"
#include <math.h>
#include "Game/Sfx.h"
#include "Entity/DevCar.h"
#include "Game/Util.h"
#include "Game/Game.h"
#include "Game/Controller.h"

namespace nyaa {

void New3DRenderer::renderFloorTiles(Map* map, RendTarget& target)
{
    // Follow player's wave motion when drug effect is active
    float waveOffset = 0.0f;
    if (wavyWorldManager && wavyWorldManager->getCurrentEffect() > 0.0f) {
        float time = shaderClock.getElapsedSeconds();
        double playerX = IGS->player->posX / 16.0;
        double playerZ = IGS->player->posY / 16.0;
        float effectStrength = wavyWorldManager->getCurrentEffect();
        waveOffset = (float)(sin(playerX * 0.1 + (double)time * 2.0) * 2.0 * (double)effectStrength +
                     cos(playerZ * 0.1 + (double)time * 1.5) * 1.5 * (double)effectStrength);
    }
    
    // We already called update() in setupCamera, so just call use() here if we need to apply effects like waveOffset
    // Actually, setupCamera is called before renderFloorTiles, so we can just re-apply 'use' with the waveOffset
    m_camera.use(target, waveOffset);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Disable lighting, use full brightness
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    for (auto c : map->chunks)
    {
        double chunkX = (double)c->x * (double)MapChunk::SizeTiles * (double)tileSize;
        double chunkZ = (double)c->y * (double)MapChunk::SizeTiles * (double)tileSize;
        
        for (unsigned int y = 0; y < MapChunk::SizeTiles; ++y)
        {
            for (unsigned int x = 0; x < MapChunk::SizeTiles; ++x)
            {
                float relWorldX = (float)(chunkX + (double)x * (double)tileSize - camWorldX);
                float relWorldZ = (float)(chunkZ + (double)y * (double)tileSize - camWorldZ);
                
                // Floor layer tiles
                auto& floorTile = c->getTILE(x, y, 0);
                if (floorTile.id != 0)
                {
                    auto coords = sf::Vector2f{};
                    map->sheet.getTexCoordsFromId(floorTile.id, &coords.x, &coords.y);
                    float u1 = coords.x / map->sheet.tex.getSize().x;
                    float v1 = coords.y / map->sheet.tex.getSize().y;
                    float u2 = (coords.x + (float)Map::TileSizePixels) / map->sheet.tex.getSize().x;
                    float v2 = (coords.y + (float)Map::TileSizePixels) / map->sheet.tex.getSize().y;
                    
                    sf::Texture::bind(&map->sheet.tex);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glEnable(GL_TEXTURE_2D);
                    glColor3f(1.0f, 1.0f, 1.0f);
                    glNormal3f(0.0f, 1.0f, 0.0f);
                    
                    glBegin(GL_QUADS);
                    glTexCoord2f(u1, v1); glVertex3f(relWorldX, 0, relWorldZ);
                    glTexCoord2f(u2, v1); glVertex3f(relWorldX + tileSize, 0, relWorldZ);
                    glTexCoord2f(u2, v2); glVertex3f(relWorldX + tileSize, 0, relWorldZ + tileSize);
                    glTexCoord2f(u1, v2); glVertex3f(relWorldX, 0, relWorldZ + tileSize);
                    glEnd();
                    
                    glDisable(GL_TEXTURE_2D);
                }
                
                // Layer 1 obstacles are now handled in the sorted rendering section
                
                // Layer 2 ceiling tiles are now handled after sorted rendering
            }
        }
    }
    
    
#if DEBUG
    // Render 3D portals after floor
    // Moved to renderPortals()
#endif
    
}

}