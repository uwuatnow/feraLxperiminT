#pragma once
#include "Game/Globals.h"
#include <string>
#include <SFML/Graphics/Sprite.hpp>
#include "Texture/TextureMan.h"
#include "Entity/Prop.h"

namespace nyaa {

class Item : public Prop
{
public:
	Item(std::string name, std::string description = "", std::string texture = "", 
		unsigned int textureSizeFramesX = 1, unsigned int textureSizeFramesY = 1,
		unsigned int defaultTextureFrameIndex = 0,
		unsigned int sizeX = 1, unsigned int sizeY = 1,
		Direction anchor = Direction_Center
	);

	virtual ~Item();

public:
	void equip();

	virtual void use();

	virtual void equippedUpdate();

	void markForDeletion();

public:
	unsigned int defaultTextureFrameIndex;
	unsigned int textureSizeFramesX, textureSizeFramesY;
	TextureMan::Tex* tex;
	class Inv* inv;
	sf::Sprite sp;
	float invSpriteOffsetX, invSpriteOffsetY;
	std::string name, description;
	bool deleteMe;
};

}
