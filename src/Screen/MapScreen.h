#pragma once
#include "Screen/Screen.h"
#include <SFML/Graphics/View.hpp>

namespace nyaa {

class MapScreen : public Screen
{
public:
    MapScreen();
    virtual ~MapScreen();

    void doTick(RendTarget* renderTarget) override;
    bool onEnter() override;
    bool onLeave() override;

    static MapScreen* Instance;

private:
    sf::View mapView;
    float zoomLevel;
    sf::Vector2f mapCenter;
    sf::Vector2f lastMousePos;
    bool dragging;
};

}
