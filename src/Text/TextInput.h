#pragma once
#include "Game/Globals.h"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "Game/Clock.h"
#include "Game/GuiWidget.h"

namespace nyaa {

class TextInput : public GuiWidget
{
public:
	TextInput(float posX, float posY, std::string nname, std::string defaultInput);
	
public:
	void update();
	
	void render(RendTarget* renderTarget);
	
	void reset();
	
	void backspc();
	
	void setNameOrigin();

	void setName(std::string name);

	void getCharactersLocalBounds(float* outLeft, float* outTop, float* outWidth, float* outHeight);

	void getCharactersGlobalBounds(float* outLeft, float* outTop, float* outWidth, float* outHeight);

private:
	sf::Text name;
	sf::Text characters;
public:
	sf::Text input;
	sf::RectangleShape selRect;
	int selIndex;
	bool typing;
	bool selRectOpacityUp;
	Clock selRectOpacityClock;
	int selRectOpacityBreatheMs;
};

}