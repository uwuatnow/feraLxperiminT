#include "Entity/Door.h"
#include "Entity/Actor.h"
#include "Map/Map.h"
#include "Game/Sfx.h"
#include "Entity/Interaction.h"
#include "Game/Util.h"
#include <cmath>
#include <SFML/System/Vector2.hpp>
#include "Game/Game.h"

namespace nyaa {

Door::Door()
{
	flags |= EntFlag_UnderAllOtherEnts;
	dirAngle = 0.0f; //front of Door faces Z+ (camera always points at Z- 3D)
	sizeX = 16.0f;
	sizeY = 3.0f;
}

void Door::update()
{
	Entity::update();
}

}