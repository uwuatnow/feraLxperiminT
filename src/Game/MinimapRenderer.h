#pragma once
#include "Game/Globals.h"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include "Game/Clock.h"

#include "Game/GuiWidget.h"

namespace nyaa {

class Map;
class Entity;

class MinimapRenderer : public GuiWidget
{
public:
    MinimapRenderer(float sizeX, float sizeY, float positionX, float positionY);
    ~MinimapRenderer();

    void update(Map* map, Entity* player, bool autoWalking = false, 
        double autoWalkTargetX = 0, double autoWalkTargetY = 0,
        double mousePosOnMapX = 0, double mousePosOnMapY = 0);
    void render(RendTarget& target);
    void startSlideInAnimation();
    
    float getPositionX() const { return positionX; }
    float getPositionY() const { return positionY; }
    float getSizeX() const { return sizeX; }
    float getSizeY() const { return sizeY; }

private:
    sf::RenderTexture minimapTexture;
    sf::Sprite minimapSprite;
    float sizeX, sizeY;
    float positionX, positionY;
    float originalPositionX, originalPositionY;
    sf::RectangleShape background;
    sf::CircleShape entityDot;
    sf::CircleShape playerDot;
    sf::CircleShape border;
    sf::CircleShape mouseDot;
    ShaderProg circularShader;
    
    float scale;
    double mapCenterX, mapCenterY;
    
    Clock slideInClock;
    bool slideInActive;
    static const float SLIDE_IN_DURATION;
};

}