#pragma once
#include "Game/Globals.h"

#include <string>

namespace nyaa {

class Entity;
class Map;
class Interaction;

class Portal
{
public:
	Portal(int tx, int ty);
	
public:
	void activate(Entity* user);

public:
	int tilePosX, tilePosY;
	std::string name;
	std::string destMapName;
	std::string destPortalName;
	Map* hostMap;
	Interaction* editInteraction;
	int outDirAngleDeg;
	int outWalkAmount;
};

}