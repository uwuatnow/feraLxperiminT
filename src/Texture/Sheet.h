#pragma once
#include "Game/Globals.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <string>

namespace nyaa {

class Sheet
{
public:
	Sheet(std::string loc, unsigned int sizeX, unsigned int sizeY);

	virtual ~Sheet();

public:
	void getTexCoordsFromId(unsigned int tileId, float* outX, float* outY);

	unsigned int getId(unsigned int posX, unsigned int posY);

#if DEBUG
	void reload();
#endif

private:
	void loadAndPad(const std::string& path);

public:
	unsigned int sizeX, sizeY; //tiles
	unsigned int tileSizePixelsX, tileSizePixelsY;
	std::string loc;
	sf::Texture tex;
};

}
