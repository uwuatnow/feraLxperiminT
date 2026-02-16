#include "Potty.h"
#include "Entity/Actor.h"
#include "Screen/InGameScreen.h"
//#include "Game/Game.h"
#include "Game/Util.h"
#include "Entity/Interaction.h"
#include <iostream>
#include <cassert>
#include "Game/Game.h"
#include "Mission/FirstMission.h"
#include "Screen/InGameScreen.h"

namespace nyaa {

Potty::Potty(double urineCapacity)
	:Prop("toilet", 1, 1)
	,urineCapacity(urineCapacity)
	,urineAmount(0)
{
	originX = 0.5f;
	originY = 0.75f;
	friction = 0.02f;
	nearInteractHovArrowPosOffsetX = 0.0f;
	nearInteractHovArrowPosOffsetY = -9.0f;
	auto usePottyInter = new Interaction(this, "Use potty", [this](Interaction* in) -> bool
	{
		if (Actor* a = dynamic_cast<Actor*>(in->user))
		{
			a->bladder = 0;
		}
		else assert(0);
		if (FirstMission* fm = dynamic_cast<FirstMission*>(IGS->curMission))
		{
			fm->pottyUsed = true;
		}
		//else assert(0);
		return true;
	});
	usePottyInter->priorityLevel = 2;
	interactions.push_back(usePottyInter);
}

Potty::~Potty()
{
	if(urineAmount > urineCapacity)
	{
		std::cout << "urine debt!!" << std::endl;
	}
}

void Potty::addUrine(double urineAmount)
{

}

void Potty::update()
{
	//dir = Direction_Down;
	//dirAngle = 90;
	Prop::update();
}

bool Potty::onInteract(Actor* user)
{
	return false;
}


}