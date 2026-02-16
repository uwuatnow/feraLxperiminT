#pragma once
#include "Game/Globals.h"
#include "Inventory/Item.h"

namespace nyaa {

class Drug : public Item
{
public:
	Drug(std::string name, std::string description = "", std::string texture = "", 
		unsigned int textureSizeFramesX = 1, unsigned int textureSizeFramesY = 1,
		unsigned int defaultTextureFrameIndex = 0,
		unsigned int sizeX = 1, unsigned int sizeY = 1,
		Direction anchor = Direction_Center);

    virtual ~Drug();
};

}
