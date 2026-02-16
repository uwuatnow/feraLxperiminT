#pragma once
#include "Game/Globals.h"
#include "Map/TileFlags.h"

namespace nyaa {

class MapTile
{
public:
	MapTile();

public:
	TileFlags rotation;
	unsigned char colorR, colorG, colorB, colorA;
	unsigned short id;
};

}