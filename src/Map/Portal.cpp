#include "Map/Map.h"
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include "Entity/CarBase.h"
#include "Map/MapFactory.h"
#include <cassert>

namespace nyaa {

Portal::Portal(int tx, int ty)
	:tilePosX(tx)
	,tilePosY(ty)
	,hostMap(nullptr)
	,editInteraction(nullptr)
	,outDirAngleDeg(0)
	,outWalkAmount(0)
{
	
}

void Portal::activate(Entity* user)
{
	if(user == IGS->player) {
		IGS->initialCam = true;
		
	}
	// if (user != IGS->player)
	// {
	// 	return;
	// }
	
	// Disallow cars from using portals
	if (dynamic_cast<CarBase*>(user)) {
		return;
	}
	
	auto toMap = IGS->mapFactory->find(destMapName);
	if (toMap)
	{
		assert(hostMap);
		assert(user->hostMap);
		if (user->hostMap == hostMap)
		{
			auto portal = toMap->getPortal(destPortalName);
			sf::Vector2i tp = {0,0};
			if (portal)
			{
				tp = { portal->tilePosX, portal->tilePosY };
			}
			else
			{
				std::printf("Dest portal not found. spawning at 0,0\n");
			}
			toMap->moveEntHere(user, tp.x, tp.y);
			if (portal) {
				user->physicsSlideAmount = portal->outWalkAmount;
				user->physicsSlideAngle = portal->outDirAngleDeg;
			}
			// // Simulate destination map for non-player entities
			// if (user != IGS->player) {
			// 	for (int i = 0; i < 5; i++) {
			// 		toMap->update();
			// 	}
			// }
		}
	}
}

}