#include "Game/MinimapRenderer.h"
#include "Map/Map.h"
#include "Map/MapChunk.h"
#include "Entity/Entity.h"
#include "Entity/Actor.h"
#include "Entity/Road.h"
#include "Game/Game.h"
#include "Texture/Sheet.h"
#include "Map/Pathfinding.h"
#include "Game/Mouse.h"
#include "Game/Util.h"

namespace nyaa {

const float MinimapRenderer::SLIDE_IN_DURATION = 0.5f;

MinimapRenderer::MinimapRenderer(float sizeX, float sizeY, float positionX, float positionY)
    :sizeX(sizeX)
    ,sizeY(sizeY)
    ,positionX(positionX)
    ,positionY(positionY)
    ,originalPositionX(positionX)
    ,originalPositionY(positionY)
    ,scale(0.05f)
    ,mapCenterX(0.0f)
    ,mapCenterY(0.0f)
    ,slideInActive(false)
{
    minimapTexture.create((unsigned int)sizeX, (unsigned int)sizeY);
    minimapSprite.setTexture(minimapTexture.getTexture());
    minimapSprite.setPosition(positionX, positionY);
    
    background.setSize({ sizeX, sizeY });
    background.setFillColor(sf::Color(0, 0, 0, 150));
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(2.0f);
    
    entityDot.setRadius(2.0f);
    entityDot.setFillColor(sf::Color::Red);
    entityDot.setOrigin(2.0f, 2.0f);
    
    playerDot.setRadius(3.0f);
    playerDot.setFillColor(sf::Color::Green);
    playerDot.setOrigin(3.0f, 3.0f);
    
    float radius = sizeX * 0.5f;
    border.setRadius(radius);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::White);
    border.setOutlineThickness(3.0f);
    border.setOrigin(radius, radius);
    border.setPosition(positionX + radius, positionY + radius);
    
    mouseDot.setRadius(2.0f);
    mouseDot.setFillColor(sf::Color::Cyan);
    mouseDot.setOrigin(2.0f, 2.0f);
    
    const std::string fragmentShader = 
        "uniform sampler2D texture;"
        "uniform vec2 center;"
        "uniform float radius;"
        "void main() {"
        "    vec2 pos = gl_TexCoord[0].xy;"
        "    float dist = distance(pos, center);"
        "    if (dist > radius) discard;"
        "    gl_FragColor = texture2D(texture, pos) * gl_Color;"
        "}";
    circularShader.loadFromMemory(fragmentShader, sf::Shader::Fragment);
    circularShader.setUniform("texture", sf::Shader::CurrentTexture);
    circularShader.setUniform("center", sf::Vector2f(0.5f, 0.5f));
    circularShader.setUniform("radius", 0.5f);
}

MinimapRenderer::~MinimapRenderer()
{
}

void MinimapRenderer::update(Map* map, Entity* player, bool autoWalking,
    double autoWalkTargetX, double autoWalkTargetY,
    double mousePosOnMapX, double mousePosOnMapY)
{
    if (!map || !player) return;
    
    mapCenterX = player->posX;
    mapCenterY = player->posY;
    
    minimapTexture.clear(sf::Color(50, 50, 50, 200));
    minimapTexture.draw(background);
    
    // Draw floor tiles
    sf::VertexArray tileVA(sf::PrimitiveType::Quads);
    for (auto chunk : map->chunks)
    {
        int cx = chunk->x;
        int cy = chunk->y;
        
        // Skip chunks that are obviously outside the minimap view to save processing
        double chunkWorldX = (double)cx * MapChunk::SizePixels;
        double chunkWorldY = (double)cy * MapChunk::SizePixels;
        float chunkScreenX = sizeX * 0.5f + (float)((chunkWorldX - mapCenterX) * scale);
        float chunkScreenY = sizeY * 0.5f + (float)((chunkWorldY - mapCenterY) * scale);
        float chunkDim = (float)MapChunk::SizePixels * scale;
        
        if (chunkScreenX + chunkDim < -32.0f || chunkScreenX > sizeX + 32.0f ||
            chunkScreenY + chunkDim < -32.0f || chunkScreenY > sizeY + 32.0f)
        {
            continue;
        }

        for (int ty = 0; ty < (int)MapChunk::SizeTiles; ty++)
        {
            for (int tx = 0; tx < (int)MapChunk::SizeTiles; tx++)
            {
                auto& tile = chunk->getTILE(tx, ty, 0); // Floor layer
                if (tile.id == 0) continue;
                
                double worldPosX = chunkWorldX + (double)tx * 16.0;
                double worldPosY = chunkWorldY + (double)ty * 16.0;
                
                sf::Vector2f minimapPos(
                    sizeX * 0.5f + (float)((worldPosX - mapCenterX) * scale),
                    sizeY * 0.5f + (float)((worldPosY - mapCenterY) * scale)
                );
                
                if (minimapPos.x >= -16 && minimapPos.x <= sizeX + 16 && 
                    minimapPos.y >= -16 && minimapPos.y <= sizeY + 16)
                {
                    float tileSize = 16.0f * scale;
                    auto texCoords = sf::Vector2f{};
                    map->sheet.getTexCoordsFromId(tile.id, &texCoords.x, &texCoords.y);
                    
                    tileVA.append(sf::Vertex(minimapPos, sf::Color{tile.colorR, tile.colorG, tile.colorB, tile.colorA}, texCoords));
                    tileVA.append(sf::Vertex(sf::Vector2f(minimapPos.x + tileSize, minimapPos.y), sf::Color{tile.colorR, tile.colorG, tile.colorB, tile.colorA}, sf::Vector2f(texCoords.x + 16, texCoords.y)));
                    tileVA.append(sf::Vertex(sf::Vector2f(minimapPos.x + tileSize, minimapPos.y + tileSize), sf::Color{tile.colorR, tile.colorG, tile.colorB, tile.colorA}, sf::Vector2f(texCoords.x + 16, texCoords.y + 16)));
                    tileVA.append(sf::Vertex(sf::Vector2f(minimapPos.x, minimapPos.y + tileSize), sf::Color{tile.colorR, tile.colorG, tile.colorB, tile.colorA}, sf::Vector2f(texCoords.x, texCoords.y + 16)));
                }
            }
        }
    }
    minimapTexture.draw(tileVA, &map->sheet.tex);
    
    // Draw roads
    for (Entity* entity : map->getEntities())
    {
        if (Road* road = dynamic_cast<Road*>(entity))
        {
            if (road->anchors.size() >= 2)
            {
                // Generate road mesh with moderate resolution for minimap
                RoadMesh mesh = road->generateCatmullRoadMesh(16);
                
                sf::VertexArray asphaltVA(sf::PrimitiveType::Triangles);
                sf::VertexArray linesVA(sf::PrimitiveType::LineStrip);
                
                sf::Color asphaltColor(road->asphaltColorR, road->asphaltColorG, road->asphaltColorB);
                sf::Color linesColor(road->linesColorR, road->linesColorG, road->linesColorB);
                
                // Draw asphalt mesh
                for (size_t i = 0; i < mesh.indices.size(); i += 3)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        auto& v = mesh.vertices[mesh.indices[i + j]];
                        // RoadVertex uses x and z for the horizontal plane
                        sf::Vector2f minimapPos(
                            sizeX * 0.5f + (float)((v.position.x - mapCenterX) * scale),
                            sizeY * 0.5f + (float)((v.position.z - mapCenterY) * scale)
                        );
                        asphaltVA.append(sf::Vertex(minimapPos, asphaltColor));
                    }
                }
                
                minimapTexture.draw(asphaltVA);

                // Draw asphalt circles at anchors to smooth joints (consistent with in-game renderer)
                float anchorRadius = (float)road->width * 0.5f * scale;
                for (const auto& anchor : road->anchors)
                {
                    sf::Vector2f minimapPos(
                        sizeX * 0.5f + (float)((anchor.x * 16.0 - mapCenterX) * scale),
                        sizeY * 0.5f + (float)((anchor.y * 16.0 - mapCenterY) * scale)
                    );
                    
                    sf::CircleShape joint(anchorRadius);
                    joint.setFillColor(asphaltColor);
                    joint.setOrigin(anchorRadius, anchorRadius);
                    joint.setPosition(minimapPos);
                    minimapTexture.draw(joint);
                }
                
                // Draw yellow center line
                // RoadMesh vertices are added in pairs (left edge, right edge)
                for (size_t i = 0; i < mesh.vertices.size(); i += 2)
                {
                    const auto& vL = mesh.vertices[i];
                    const auto& vR = mesh.vertices[i + 1];
                    
                    // Midpoint between left and right edge is the center line
                    sf::Vector2f minimapPos(
                        sizeX * 0.5f + (float)(((vL.position.x + vR.position.x) * 0.5 - mapCenterX) * scale),
                        sizeY * 0.5f + (float)(((vL.position.z + vR.position.z) * 0.5 - mapCenterY) * scale)
                    );
                    
                    linesVA.append(sf::Vertex(minimapPos, linesColor));
                }
                
                minimapTexture.draw(linesVA);
            }
        }
    }
    
    // Draw entities
    for (auto& entity : map->getEntities())
    {
        if (entity == player) continue;
        if (entity->flags & Entity::EntFlag_Dead) continue;
        
        sf::Vector2f minimapPos(
            sizeX * 0.5f + (float)((entity->posX - mapCenterX) * scale),
            sizeY * 0.5f + (float)((entity->posY - mapCenterY) * scale)
        );
        
        if (minimapPos.x >= 0 && minimapPos.x <= sizeX && 
            minimapPos.y >= 0 && minimapPos.y <= sizeY)
        {
            entityDot.setPosition(minimapPos);
            minimapTexture.draw(entityDot);
        }
    }
    
    // Draw player at center
    playerDot.setPosition(sizeX * 0.5f, sizeY * 0.5f);
    minimapTexture.draw(playerDot);
    
    // Draw mouse position
    sf::Vector2f mouseMinimap(
        sizeX * 0.5f + (float)((mousePosOnMapX - mapCenterX) * scale),
        sizeY * 0.5f + (float)((mousePosOnMapY - mapCenterY) * scale)
    );
    mouseDot.setPosition(mouseMinimap);
    minimapTexture.draw(mouseDot);
    
    // Draw collision lines
    sf::VertexArray collisionVA(sf::PrimitiveType::Lines);
    for (CollisionLine* collLine : map->collLines)
    {
        sf::Vector2f p1Minimap(
            sizeX * 0.5f + (float)((collLine->p1.x - mapCenterX) * scale),
            sizeY * 0.5f + (float)((collLine->p1.y - mapCenterY) * scale)
        );
        sf::Vector2f p2Minimap(
            sizeX * 0.5f + (float)((collLine->p2.x - mapCenterX) * scale),
            sizeY * 0.5f + (float)((collLine->p2.y - mapCenterY) * scale)
        );
        
        collisionVA.append(sf::Vertex(p1Minimap, sf::Color::White));
        collisionVA.append(sf::Vertex(p2Minimap, sf::Color::White));
    }
    minimapTexture.draw(collisionVA);
    
    // Draw auto-walk path and destination
    if (autoWalking)
    {
        // Draw destination dot
        sf::Vector2f destMinimap(
            sizeX * 0.5f + (float)((autoWalkTargetX - mapCenterX) * scale),
            sizeY * 0.5f + (float)((autoWalkTargetY - mapCenterY) * scale)
        );
        
        sf::CircleShape destDot(4.0f);
        destDot.setFillColor(sf::Color::Yellow);
        destDot.setOrigin(4.0f, 4.0f);
        destDot.setPosition(destMinimap);
        minimapTexture.draw(destDot);
        
        // Draw path if available
        if (auto actor = dynamic_cast<Actor*>(player))
        {
            if (actor->ast && actor->ast->valid())
            {
                sf::VertexArray pathVA(sf::PrimitiveType::LineStrip);
                PathfindNode* node = actor->ast->startNode;
                while (node)
                {
                    sf::Vector2f nodeMinimap(
                        sizeX * 0.5f + (float)((node->posX - mapCenterX) * scale),
                        sizeY * 0.5f + (float)((node->posY - mapCenterY) * scale)
                    );
                    pathVA.append(sf::Vertex(nodeMinimap, sf::Color::Cyan));
                    node = node->child;
                }
                minimapTexture.draw(pathVA);
            }
        }
    }
    
    minimapTexture.display();
}

void MinimapRenderer::render(RendTarget& target)
{
    sf::Vector2f renderPosition = { positionX, positionY };
    
    if (slideInActive) {
        float elapsed = slideInClock.getElapsedSeconds();
        if (elapsed < SLIDE_IN_DURATION) {
            float t = elapsed / SLIDE_IN_DURATION;
            t = t * t * (3.0f - 2.0f * t); // smoothstep
            
            sf::Vector2f startPos = sf::Vector2f{ originalPositionX, originalPositionY } + sf::Vector2f(sizeX, 0.0f);
            renderPosition = startPos + (sf::Vector2f{ originalPositionX, originalPositionY } - startPos) * t;
            
            minimapSprite.setPosition(renderPosition);
            border.setPosition(renderPosition.x + sizeX * 0.5f, renderPosition.y + sizeY * 0.5f);
        } else {
            slideInActive = false;
            renderPosition = sf::Vector2f{ originalPositionX, originalPositionY };
            minimapSprite.setPosition(renderPosition);
            border.setPosition(renderPosition.x + sizeX * 0.5f, renderPosition.y + sizeY * 0.5f);
        }
    }
    
    target.draw(minimapSprite, &circularShader);
    target.draw(border);
    
    // Check if mouse is within minimap radius and set guip
    sf::Vector2f center = renderPosition + sf::Vector2f(sizeX * 0.5f, sizeY * 0.5f);
    float radius = sizeX * 0.5f;
    float mouseDistance = Util::Dist(Mouse::Pos_X, Mouse::Pos_Y, center.x, center.y);
    if (mouseDistance <= radius) {
        guip = (GuiWidget*)this;
    }
}

void MinimapRenderer::startSlideInAnimation()
{
    slideInActive = true;
    slideInClock.restart();
}

}
