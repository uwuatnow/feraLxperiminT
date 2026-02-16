#include "Screen/MapSwitchScreen.h"
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include "Map/MapFactory.h"
#include "Map/Map.h"

namespace nyaa {

MapSwitchScreen* MapSwitchScreen::Instance = nullptr;

MapSwitchScreen::MapSwitchScreen()
{
	Instance = this;
}

bool MapSwitchScreen::onEnter()
{
	//do stuff then go back to the prev screen
	if (IGS->mapFactory->switchTo(IGS->switchToMapName))
	{
		//std::printf("switch was good!\n");
		auto portalUsed = IGS->mcPortalUsed;
		if (portalUsed)
		{
			portalUsed->activate(IGS->player);
			IGS->mcPortalUsed = nullptr;
		}

		auto toMap = IGS->mapFactory->find(IGS->switchToMapName);
		if (toMap && IGS->player->hostMap != toMap) {
			sf::Vector2i tp;
			if (portalUsed) tp = { portalUsed->tilePosX, portalUsed->tilePosY };
			toMap->moveEntHere(IGS->player, toMap->spawn_tileX * (int)Map::TileSizePixels, toMap->spawn_tileY * (int)Map::TileSizePixels);
		}

		// Reset auto-walking state
		IGS->playerAutoWalking = false;
		IGS->player->isControllable = true;
		IGS->player->healEnergy();
	}
	return false; //to not stay on this screen
}

}