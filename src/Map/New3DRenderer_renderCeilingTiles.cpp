#include "Map/New3DRenderer.h"
#include "Map/Map.h"
#include "Map/MapChunk.h"
#include "Texture/Sheet.h"
#include <SFML/OpenGL.hpp>

namespace nyaa {

void New3DRenderer::renderCeilingTiles(Map* map, RendTarget& target)
{
    // Render layer 2 ceiling tiles on top
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    for (auto c : map->chunks) {
        double chunkX = (double)c->x * (double)MapChunk::SizeTiles * (double)tileSize;
        double chunkZ = (double)c->y * (double)MapChunk::SizeTiles * (double)tileSize;
        
        for (unsigned int y = 0; y < MapChunk::SizeTiles; ++y) {
            for (unsigned int x = 0; x < MapChunk::SizeTiles; ++x) {
                auto& ceilingTile = c->getTILE(x, y, 2);
                if (ceilingTile.id != 0) {
                    float relWorldX = (float)(chunkX + (double)x * (double)tileSize - camWorldX);
                    float relWorldZ = (float)(chunkZ + (double)y * (double)tileSize - camWorldZ);
                    
                    auto coords = sf::Vector2f{};
                    map->sheet.getTexCoordsFromId(ceilingTile.id, &coords.x, &coords.y);
                    float u1 = coords.x / map->sheet.tex.getSize().x;
                    float v1 = coords.y / map->sheet.tex.getSize().y;
                    float u2 = (coords.x + (float)Map::TileSizePixels) / map->sheet.tex.getSize().x;
                    float v2 = (coords.y + (float)Map::TileSizePixels) / map->sheet.tex.getSize().y;
                    
                    sf::Texture::bind(&map->sheet.tex);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glEnable(GL_TEXTURE_2D);
                    glColor3f(1.0f, 1.0f, 1.0f);
                    glNormal3f(0.0f, -1.0f, 0.0f);
                    
                    glBegin(GL_QUADS);
                    glTexCoord2f(u1, v1); glVertex3f(relWorldX, 2, relWorldZ);
                    glTexCoord2f(u2, v1); glVertex3f(relWorldX + tileSize, 2, relWorldZ);
                    glTexCoord2f(u2, v2); glVertex3f(relWorldX + tileSize, 2, relWorldZ + tileSize);
                    glTexCoord2f(u1, v2); glVertex3f(relWorldX, 2, relWorldZ + tileSize);
                    glEnd();
                    
                    glDisable(GL_TEXTURE_2D);
                }
            }
        }
    }
    
}

}