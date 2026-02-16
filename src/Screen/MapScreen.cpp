#include "Screen/MapScreen.h"
#include "Screen/InGameScreen.h"
#include "Game/Game.h"
#include "Game/Mouse.h"
#include "Game/Kb.h"
#include "Game/Controller.h"
#include "Map/MapFactory.h"
#include "Map/Map.h"
#include "Entity/Actor.h"
#include "Game/Fonts.h"
#include "Game/Util.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Mouse.hpp>
#include "Texture/Sheet.h"
#include "Entity/Road.h"
#include "Game/Util.h"

namespace nyaa {

MapScreen* MapScreen::Instance = nullptr;

MapScreen::MapScreen()
    : zoomLevel(1.0f)
    , dragging(false)
{
    Instance = this;
    mapView.setSize((float)Game::ScreenWidth, (float)Game::ScreenHeight);
}

MapScreen::~MapScreen()
{
}

void MapScreen::doTick(RendTarget* renderTarget)
{
    if (!IGS || !IGS->player) return;

    IGS->otherScreenUpdate();

    // Handle Input
    sf::Vector2f mousePos((float)Mouse::Pos_X, (float)Mouse::Pos_Y);
    
    // Zooming
    if (Mouse::WheelDelta != 0)
    {
        float zoomFactor = (Mouse::WheelDelta > 0) ? 0.8f : 1.25f;
        zoomLevel *= zoomFactor;
        mapView.zoom(zoomFactor);
    }

    // WASD Panning
    float panSpeed = 500.0f * zoomLevel * (G->frameDeltaMillis / 1000.0f);
    if (Kb::IsKeyDown(KB::W)) mapView.move(0, -panSpeed);
    if (Kb::IsKeyDown(KB::S)) mapView.move(0, panSpeed);
    if (Kb::IsKeyDown(KB::A)) mapView.move(-panSpeed, 0);
    if (Kb::IsKeyDown(KB::D)) mapView.move(panSpeed, 0);

    // Dragging
    if (Mouse::RightFrames > 0 || Mouse::MiddleFrames > 0)
    {
        if (!dragging)
        {
            dragging = true;
            lastMousePos = mousePos;
        }
        else
        {
            sf::Vector2f delta = lastMousePos - mousePos;
            mapView.move(delta.x * zoomLevel, delta.y * zoomLevel);
            lastMousePos = mousePos;
        }
    }
    else
    {
        dragging = false;
    }

    // Back to game
    if (Kb::IsKeyFirstFrame(KB::Escape) || Kb::IsKeyFirstFrame(KB::M) || Controller::BtnFrames[Btn_Circle] == 1)
    {
        IGS->switchTo();
        return;
    }

    // Render
    renderTarget->clear(sf::Color(20, 20, 20));
    
    sf::View oldView = renderTarget->getView();
    renderTarget->setView(mapView);
    
    Map* map = IGS->mapFactory->getCurrentMap();
    if (map)
    {
        sf::FloatRect viewRect(
            mapView.getCenter().x - mapView.getSize().x / 2.0f,
            mapView.getCenter().y - mapView.getSize().y / 2.0f,
            mapView.getSize().x,
            mapView.getSize().y
        );

        // Load chunks in view
        int startCX = (int)floor(viewRect.left / MapChunk::SizePixels);
        int startCY = (int)floor(viewRect.top / MapChunk::SizePixels);
        int endCX = (int)ceil((viewRect.left + viewRect.width) / MapChunk::SizePixels);
        int endCY = (int)ceil((viewRect.top + viewRect.height) / MapChunk::SizePixels);

        for (int cy = startCY; cy <= endCY; cy++)
        {
            for (int cx = startCX; cx <= endCX; cx++)
            {
                auto distFromPlayer = (float)Util::Dist(IGS->player->posX, IGS->player->posY, cx * MapChunk::SizePixels, cy * MapChunk::SizePixels);
                if (distFromPlayer > 1500) continue;
                map->getChunk(cx, cy, true);
            }
        }

        // Simple chunk-based rendering for the map
        // We'll draw floor tiles (layer 0)
        sf::VertexArray tileVA(sf::PrimitiveType::Quads);
        
        for (auto chunk : map->chunks)
        {
            float chunkWorldX = (float)chunk->x * MapChunk::SizePixels;
            float chunkWorldY = (float)chunk->y * MapChunk::SizePixels;
            
            if (chunkWorldX + MapChunk::SizePixels < viewRect.left || chunkWorldX > viewRect.left + viewRect.width ||
                chunkWorldY + MapChunk::SizePixels < viewRect.top || chunkWorldY > viewRect.top + viewRect.height)
            {
                continue;
            }

            for (int ty = 0; ty < (int)MapChunk::SizeTiles; ty++)
            {
                for (int tx = 0; tx < (int)MapChunk::SizeTiles; tx++)
                {
                    auto& tile = chunk->getTILE(tx, ty, 0);
                    if (tile.id == 0) continue;
                    
                    float worldPosX = chunkWorldX + (float)tx * 16.0f;
                    float worldPosY = chunkWorldY + (float)ty * 16.0f;
                    
                    if (worldPosX + 16 < viewRect.left || worldPosX > viewRect.left + viewRect.width ||
                        worldPosY + 16 < viewRect.top || worldPosY > viewRect.top + viewRect.height)
                    {
                        continue;
                    }

                    auto texCoords = sf::Vector2f{};
                    map->sheet.getTexCoordsFromId(tile.id, &texCoords.x, &texCoords.y);
                    
                    sf::Color col(tile.colorR, tile.colorG, tile.colorB, tile.colorA);
                    
                    tileVA.append(sf::Vertex(sf::Vector2f(worldPosX, worldPosY), col, texCoords));
                    tileVA.append(sf::Vertex(sf::Vector2f(worldPosX + 16, worldPosY), col, sf::Vector2f(texCoords.x + 16, texCoords.y)));
                    tileVA.append(sf::Vertex(sf::Vector2f(worldPosX + 16, worldPosY + 16), col, sf::Vector2f(texCoords.x + 16, texCoords.y + 16)));
                    tileVA.append(sf::Vertex(sf::Vector2f(worldPosX, worldPosY + 16), col, sf::Vector2f(texCoords.x, texCoords.y + 16)));
                }
            }
        }
        renderTarget->draw(tileVA, &map->sheet.tex);


        // Draw roads
        for (Entity* entity : map->getEntities())
        {
            if (Road* road = dynamic_cast<Road*>(entity))
            {
                if (road->anchors.size() >= 2)
                {
                    RoadMesh mesh = road->generateCatmullRoadMesh(16);
                    sf::VertexArray asphaltVA(sf::PrimitiveType::Triangles);
                    sf::Color asphaltColor(road->asphaltColorR, road->asphaltColorG, road->asphaltColorB);
                    
                    for (size_t i = 0; i < mesh.indices.size(); i += 3)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            auto& v = mesh.vertices[mesh.indices[i + j]];
                            asphaltVA.append(sf::Vertex(sf::Vector2f((float)v.position.x, (float)v.position.z), asphaltColor));
                        }
                    }
                    renderTarget->draw(asphaltVA);

                    // Joints
                    float anchorRadius = (float)road->width * 0.5f;
                    for (const auto& anchor : road->anchors)
                    {
                        sf::CircleShape joint(anchorRadius);
                        joint.setFillColor(asphaltColor);
                        joint.setOrigin(anchorRadius, anchorRadius);
                        joint.setPosition((float)anchor.x * 16.0f, (float)anchor.y * 16.0f);
                        renderTarget->draw(joint);
                    }
                }
            }
        }
        
        // Draw player
        sf::CircleShape playerDot(12.0f * zoomLevel);
        playerDot.setFillColor(sf::Color::Green);
        playerDot.setOutlineColor(sf::Color::White);
        playerDot.setOutlineThickness(2.0f * zoomLevel);
        playerDot.setOrigin(12.0f * zoomLevel, 12.0f * zoomLevel);
        playerDot.setPosition((float)IGS->player->posX, (float)IGS->player->posY);
        renderTarget->draw(playerDot);

        
        // Draw other entities
        for (auto ent : map->getEntities())
        {
            if (ent == IGS->player) continue;
            if (ent->flags & Entity::EntFlag_Dead) continue;
            
            sf::CircleShape entDot(8.0f * zoomLevel);
            entDot.setFillColor(sf::Color::Red);
            entDot.setOrigin(8.0f * zoomLevel, 8.0f * zoomLevel);
            entDot.setPosition((float)ent->posX, (float)ent->posY);
            renderTarget->draw(entDot);
        }
    }
    
    renderTarget->setView(oldView);
    
    // UI Overlay
    sf::Text hint("WASD/MB: Pan | Scroll: Zoom | Esc/M: Back", *Fonts::OSDFont, 16);
    hint.setPosition(20, Game::ScreenHeight - 30);
    hint.setFillColor(sf::Color::White);
    hint.setOutlineColor(sf::Color::Black);
    hint.setOutlineThickness(1);
    renderTarget->draw(hint);
    
    sf::Text title("OVERWORLD MAP", *Fonts::OSDFont, 24);
    title.setPosition(Game::ScreenWidth / 2.0f - title.getLocalBounds().width / 2.0f, 20);
    title.setFillColor(sf::Color::Yellow);
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(2);
    renderTarget->draw(title);
}

bool MapScreen::onEnter()
{
    if (IGS && IGS->player)
    {
        mapCenter = sf::Vector2f((float)IGS->player->posX, (float)IGS->player->posY);
        mapView.setCenter(mapCenter);
        zoomLevel = 1.0f;
        mapView.setSize((float)Game::ScreenWidth, (float)Game::ScreenHeight);
    }
    return true;
}

bool MapScreen::onLeave()
{
    return true;
}

}
