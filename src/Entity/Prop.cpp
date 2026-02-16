#include "Entity/Prop.h"
#include <cassert>
#include "Game/Util.h"
#include "Texture/Sheet.h"

namespace nyaa {

Prop::Prop(std::string name, unsigned sizeX, unsigned sizeY, Direction anchor, bool dontLoadSheet)
	:sh(/* dontLoadSheet ? nullptr :  */new Sheet(dontLoadSheet ? "" : name, sizeX, sizeY))
	,anchor(anchor)
{
	originX = 0.5f;
	originY = 0.5f;
	initType();
}

Prop::~Prop()
{
	if (sh) delete sh;
}

void Prop::initType()
{
	sizeX = (float)sh->tileSizePixelsX;
	sizeY = (float)sh->tileSizePixelsY;
}

}