#include "Inventory/Drug/Drug.h"

namespace nyaa {

Drug::Drug(std::string name, std::string description, std::string texture, 
	unsigned int textureSizeFramesX, unsigned int textureSizeFramesY, 
	unsigned int defaultTextureFrameIndex, 
	unsigned int sizeX, unsigned int sizeY, 
	Direction anchor)
    :Item(name, description, texture, textureSizeFramesX, textureSizeFramesY, defaultTextureFrameIndex, sizeX, sizeY, anchor)
{
}

Drug::~Drug()
{
}

}
