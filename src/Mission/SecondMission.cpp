#include "Mission/SecondMission.h"
#include "Screen/InGameScreen.h"
#include "Entity/Catgirl.h"
#include "Game/Util.h"
#include "Screen/MissionFailedScreen.h"
#include "Game/Game.h"
#include "Map/MapFactory.h"
#include <iostream>
#include <cassert>

namespace nyaa {

SecondMission::SecondMission()
	:Mission("2nd Mission")
{
}

SecondMission::~SecondMission()
{
}

void SecondMission::onEntityTick(Entity* e)
{
}

void SecondMission::doScreenStatsText(std::string& stats)
{
}

void SecondMission::init()
{
	//Actor* ca = IGS->player;

	//auto k = IGS->mapFactory->find("kitchen");

	//auto b = IGS->mapFactory->find("basement");
	
}

void SecondMission::onCameraSettle() {
	auto& dbox = IGS->dbox;
	auto& ca = IGS->player;
	dbox.clear();
	dbox.add(ca, "welcome to mission #2");
	dbox.show();
}

void SecondMission::failMission()
{
	//DialogueBox& dbox = IGS->dbox;
	//Actor* ca = IGS->player;

	Mission::failMission();
}

void SecondMission::passMission()
{
	DialogueBox& dbox = IGS->dbox;
	Actor* ca = IGS->player;
	dbox.clear();
	dbox.add(ca, "You passed the second mission!");
	dbox.show();

	Mission::passMission();
}

bool SecondMission::onActorDeath(Actor* a)
{
	return false;
}

Mission::EventResponse SecondMission::onPropInteract(Prop* p, Actor* user)
{
	return EventResponse_Allow;
}

Mission* SecondMission::getNew()
{
	return new SecondMission();
}

}
