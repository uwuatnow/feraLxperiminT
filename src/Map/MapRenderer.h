#pragma once
#include "Game/Globals.h"

#include <SFML/Graphics/RenderTarget.hpp>

namespace nyaa {

class Map;

class MapRenderer
{
public:
    virtual ~MapRenderer() = default;
    virtual void render(Map* map, RendTarget& target) = 0;
};

}