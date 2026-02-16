#include "Entity/Actor.h"
#include "Game/Util.h"
#include "Screen/InGameScreen.h"
#include "Entity/BulletProjectile.h"
#include "Entity/Potty.h"
#include "Entity/DevCar.h"
#include "Entity/Door.h"
#include "Game/Game.h"
#include "Entity/UrinePuddle.h"
#include "Map/Pathfinding.h"
#include "Entity/Interactable.h"
#include "Mission/Mission.h"
#include "Inventory/Inventory.h"
#include "Inventory/Item.h"
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Inventory/Diaper.h"
#include "Texture/WalkAnim.h"
#include "Map/MapFactory.h"
#include "Map/Map.h"
#include "Game/Sfx.h"
#include "Game/Mouse.h"
#include <cassert>
#include <iostream>
#include <algorithm>
#include "Game/OBB.h"
#include <cmath>
#include "Inventory/Drug/DrugTrip.h"

namespace nyaa {

float Actor::HealthEnergyBasedMax = 100.0f;
float Actor::Gravity = 999.0f; // pixels per second squared, gravity
float Actor::JumpSpeed = 300.0f; // initial upward velocity for jump

void Actor::emptyBladder(Potty* p)
{
	if (urinePuddle)
	{
		return;
	}
	if (p)
	{
		/* into potty or diaper */
		if(Diaper* diap = dynamic_cast<Diaper*>(p))
		{
			// Using diaper
			diap->used = true;
			bladder = 0.0f;
		}
		else
		{
			// Using regular potty
			bladder = 0.0f;
		}
	}
	else
	{
		/* onto floor */
		isControllable = false;
		//up->setPosTile(tilePos.joyX, tilePos.joyY);
		UrinePuddle* up = new UrinePuddle(this);
		up->posX = this->posX;
		up->posY = this->posY;
		hostMap->addEnt(up);
		//G->Wet->play();
		if (this == IGS->player)
		{
			/*DialogueBox& db = inst->igs->dbox;
			db.show(*this, 1, "You wet yourself, better hope nobody noticed.");*/
			basedMeter += 10;
		}
		bladder = 0.0f;
	}
}

void Actor::hurt(float amount, DamageReason damageReason, Entity* damageSource)
{
	health -= amount;
	health = Util::Clamp(health, 0, HealthEnergyBasedMax);
	this->damageReason = damageReason;
	if(damageSource) this->damageSource = damageSource;
}

void Actor::healCompletely()
{
	healHealth();
	healEnergy();
	healHunger();
	emptyBladder(nullptr);
	//bladder = 0;
}

void Actor::healHealth()
{
	health = 100.0f;
	damageReason = DamageReason_Unknown;
}

void Actor::healEnergy()
{
	energy = 100.0f;
}

void Actor::healHunger()
{
	hunger = 0.0f;
}

void Actor::gotoMapInstant(std::string name)
{
	auto toMap = IGS->mapFactory->find(name);
	if (toMap && IGS->player->hostMap != toMap)
	{
		toMap->moveEntHere(IGS->player, toMap->spawn_tileX * (int)Map::TileSizePixels, toMap->spawn_tileY * (int)Map::TileSizePixels);
		IGS->mapFactory->cur_map = IGS->mapFactory->findIdx(toMap);
	}
}

void Actor::getHandPos(double& outX, double& outY)
{
	if (getDir() == Direction_Up) { outX = posX + 1.0; outY = posY - 8.0; }
	else if (getDir() == Direction_Down) { outX = posX - 4.0; outY = posY - 9.0; }
	else if (getDir() == Direction_Left || getDir() == Direction_Right) { outX = posX - 2.0; outY = posY - 8.0; }
}

void Actor::startTrip(DrugTrip* trip)
{
	if (trip) {
		activeTrips.push_back(trip);
	}
}

OBB* Actor::getOBB() const
{
	// Actors have a small collision box at their feet
	m_OBB->center = { posX, posY - 4.0 };
	m_OBB->halfExtents = { 6.0, 4.0 };
	m_OBB->angle = 0.0f; // Actors don't rotate their collision box
	return m_OBB;
}

} // namespace nyaa
