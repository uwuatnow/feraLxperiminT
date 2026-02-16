#include "Entity/Actor.h"
#include "Game/Game.h"
#include "Game/Sfx.h"
#include "Game/Util.h"
#include "Game/GuiWidget.h"
#include "Screen/InGameScreen.h"
#include <SFML/Graphics.hpp>
#include "Map/Map.h"
#include "Inventory/Inventory.h"
#include "Inventory/Item.h"
#include "Inventory/Diaper.h"
#include "Inventory/Drug/DrugTrip.h"
#include "Texture/WalkAnim.h"
#include "Entity/CarBase.h"
#include "Mission/Mission.h"
#include "Game/OBB.h"

namespace nyaa {

void Actor::updateTrips()
{
	// Update drug trips
	for (auto it = activeTrips.begin(); it != activeTrips.end();) {
		DrugTrip* trip = *it;
		trip->update();
		
		if (trip->finished) {
			delete trip;
			it = activeTrips.erase(it);
		} else {
			++it;
		}
	}
}

void Actor::update()
{
	Entity::update();

	// Z-axis physics (height)
	float dt = G->frameDeltaMillis / 1000.0f;
	bool wasInAir = posZ > 0.0f;
	velZ -= Gravity * dt;
	float targetZ = posZ + velZ * dt;

	// Check for landing on entity
	Entity* landingEntity = nullptr;
	float landingZ = -1.0f;
	for (auto e : hostMap->entities) {
		if (e == this || (e->flags & EntFlag_Dead) || !e->sizeZ) continue;
		OBB* myOBB = getOBB();
		OBB* eOBB = e->getOBB();
		Point dummy;
		if (!OBBIntersects(*myOBB, *eOBB, &dummy)) continue;
		float eTop = e->posZ;
		if (auto car = dynamic_cast<CarBase*>(e)) {
			float relY = posY - car->posY;
			eTop += (relY < 0 ? car->hoodHeight : car->roofHeight);
		} else {
			eTop += e->sizeZ;
		}
		if (posZ >= eTop && targetZ <= eTop) {
			if (!landingEntity || eTop > landingZ) {
				landingEntity = e;
				landingZ = eTop;
			}
		}
	}

	bool landed = false;
	if (landingEntity) {
		posZ = landingZ;
		velZ = 0.0f;
		landed = true;
		// Add to on top
		landingEntity->entitiesOnTop.push_back(this);
		// Remove from previous
		for (auto e : hostMap->entities) {
			if (e != landingEntity) {
				auto it = std::find(e->entitiesOnTop.begin(), e->entitiesOnTop.end(), this);
				if (it != e->entitiesOnTop.end()) e->entitiesOnTop.erase(it);
			}
		}
	} else if (targetZ < 0.0f) {
		posZ = 0.0f;
		velZ = 0.0f;
		landed = true;
		// Remove from all
		for (auto e : hostMap->entities) {
			auto it = std::find(e->entitiesOnTop.begin(), e->entitiesOnTop.end(), this);
			if (it != e->entitiesOnTop.end()) e->entitiesOnTop.erase(it);
		}
	} else {
		posZ = targetZ;
	}

	// Play landing sound if was in air and now landed
	if (wasInAir && landed) {
		Sfx::StoneRightFoot->play((float)posX - 3.0f, (float)posY);
		Sfx::StoneRightFoot->play((float)posX + 3.0f, (float)posY);
	}

	moveAccelTimer->update();
	notMovingT->update();
	energyAddT->update();
#if !DEBUG
	energyDelT->update();
#endif
	hungerAddT->update();
	hungerDelT->update();
	urineAddT->update();
	footT->update();
	visualTurnTimer->update();

	updateTrips();

	if (visualTurnTimer->millis() >= 500) {
		visualDir = visualTurnTarget;
		visualTurnTimer->zero();
	}

	inv->update();

	if (G->winFocused)
	{
		if (notMovingT->millis() > 500)
		{
			energy += ((float)energyAddT->millis()) / 400.0f;
			energy = Util::Clamp(energy, 0, HealthEnergyBasedMax);
		}

		float hungerAddVal = isControllable ?
			((float)hungerAddT->millis() / (((flags & EntFlag_Animating) ? 5000.0f : 10000.0f) * (energy < 2 ? 1.50f : 1.0f)))
			: 0;
		hunger += hungerAddVal;
		hunger = Util::Clamp(hunger, 0, HealthEnergyBasedMax);

		if (hunger >= HealthEnergyBasedMax)
		{
			//chance of instantly dying i guess?
			hurt(hungerAddVal, DamageReason_Hunger);
		}
		bladder += /*isControllable ? */((float)urineAddT->millis() / ((flags & EntFlag_Animating) ? 3000.0f : 5000.0f))/* : 0*/;
		bladder = Util::Clamp(bladder, 0, HealthEnergyBasedMax);
		//oh no
		if (bladder >= HealthEnergyBasedMax)
		{
			// Check for unused diaper first
			Diaper* unusedDiaper = nullptr;
			for(Item* i : inv->items)
			{
				if(Diaper* diap = dynamic_cast<Diaper*>(i))
				{
					if(!diap->used)
					{
						unusedDiaper = diap;
						break;
					}
				}
			}
			emptyBladder(unusedDiaper);
		}

		basedMeter = Util::Clamp(basedMeter, 0, HealthEnergyBasedMax);
	}

	energyAddT->zero();
	hungerAddT->zero();
	hungerDelT->zero();
	urineAddT->zero();

	if (inv->equippedItem)
	{
		inv->equippedItem->equippedUpdate();
	}

	float spd = 0.0f;

	if (IGS->player == this)
	{
		if (guip_eof == IGS)
		{
#if DEBUG
			//teleport
			//printf("wtf");
			if (Kb::IsKeyDown(KB::LShift) && Kb::IsKeyFirstFrame(KB::P))
			{
				//if(carImInsideOf)
				//{					
				//	carImInsideOf->posX = IGS->mousePosOnMapX;
				//	carImInsideOf->posY = IGS->mousePosOnMapY;
				//}
				posX = IGS->mousePosOnMapX;
				posY = IGS->mousePosOnMapY;
				updateTPos();
			}
#endif
			spd = control();

			// Handle auto-walking animation state
			if (IGS->playerAutoWalking)
			{
				flags |= EntFlag_Animating;
			}
		}
		// Ensure visualDir is synced for player when not controlling
		else
		{
			visualDir = getDir();
		}
	}
	else
	{
		//npc code
		if (isHomeWandering)
		{
			npcHomeWander();
		}
		else
		{
			npcFollow();
		}

		if (inv->equippedItem && follow && !(follow->flags & EntFlag_Dead))
		{
			inv->equippedItem->use();
		}

		// Ensure visualDir is synced with dir for NPCs
		visualDir = getDir();
	}

	if (health <= 0)
	{
		flags |= EntFlag_Dead;
		if (IGS->curMission->onActorDeath(this))
		{
			IGS->curMission->fail();
		}
	}

	//hitBox = FloatRect(Vector2f(pos.x - 3, pos.y - 10), Vector2f(6, 8));
	auto hitBox = sf::FloatRect{};
	double hl, ht;
	getHitBox(&hl, &ht, &hitBox.width, &hitBox.height);
	hitBox.left = (float)hl;
	hitBox.top = (float)ht;

	float ftm = ((moveAmountMax - (spd / G->frameDeltaMillis / 32)) * 130) 
	#if DEBUG
		* (this == IGS->player && Kb::IsKeyDown(KB::LShift) ? (1.f / ShiftSpeedBoostMultiplier) : 1)
	#endif
		;
	//std::cout << ftm << "\n";
	body->frame_time_ms = ftm;
	int bodyFrameBefore = body->frame;
	body->update();

	//wtf is this

	//if()
	{
		if(getDir() == Direction_Up || getDir() == Direction_Down)
		{
			if(body->frame == body->stationary_frame && bodyFrameBefore != body->frame/*  && footFrame == -1 */ && posZ == 0.0f)
			{
				Sfx::StoneRightFoot->play((float)posX, (float)posY);
				if (this == IGS->player)
				{
					footstepPlayedThisMove = true;
				}
			}
		}
		else if((flags & EntFlag_Animating) && (getDir() == Direction_Left || getDir() == Direction_Right))
		{
			if(body->frame != body->stationary_frame && body->frame != footFrame && posZ == 0.0f)
			{
				footFrame = body->frame;
				//foot = !foot;
				Sfx::StoneRightFoot->play((float)posX, (float)posY);
				if (this == IGS->player)
				{
					footstepPlayedThisMove = true;
				}
			}
		}
	}

	if(!(flags & EntFlag_Animating)) {
		footT->zero();
		footFrame = -1;
	}


	eyes->update();
	hair->update();
	clothes->update();
	tail->frame_time_ms = ftm;
	tail->update();
	outline->frame_time_ms = ftm;
	outline->update();
	energy = Util::Clamp(Util::Clamp(energy, 0, health * 1.2), 0, 100);

	//auto frontTile = getTileInFront();
	for (Entity* e : hostMap->entities)
	{
		if(!e || e == this || (flags & EntFlag_UnderAllOtherEnts) || (e->flags & EntFlag_UnderAllOtherEnts))
		{
			continue;
		}

		if (this == IGS->player)
		{
			e->updateTPos();
			if (Util::Dist(e->posX, e->posY, posX, posY) <= (float)Map::TileSizePixels)
			{
				for (auto ei : e->interactions)
				{
					IGS->interactables.push_back(ei);
				}
			}
		}
	}

	if(footstepPlayedThisMove)
	{
		isSnappingBack = false;
	}

	// Handle snapping back
	if (isSnappingBack)
	{
		double dist = Util::Dist(posX, posY, snapBackTargetX, snapBackTargetY);
		if (dist < 0.1)
		{
			posX = snapBackTargetX;
			posY = snapBackTargetY;
			isSnappingBack = false;
			updateTPos();
		}
		else if(dist < 8)
		{
			double dt = G->frameDeltaMillis / 1000.0;
			double speed = dist / 0.05;
			posX += (snapBackTargetX - posX) / dist * speed * dt;
			posY += (snapBackTargetY - posY) / dist * speed * dt;
		}
		else
		{
			isSnappingBack = false;
			updateTPos();
		}
	}


}

}