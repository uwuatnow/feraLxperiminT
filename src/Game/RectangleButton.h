#pragma once
#include "Game/Globals.h"

#include "Game/GuiWidget.h"
#include <SFML/Graphics/RectangleShape.hpp>

namespace nyaa {

class RectangleButton : public GuiWidget
{
public:
	sf::RectangleShape shape;
	
	RectangleButton(float sizeX, float sizeY) : shape({ sizeX, sizeY }) {}
	virtual ~RectangleButton() {}
};

}