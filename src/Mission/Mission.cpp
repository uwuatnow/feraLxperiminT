#include "Mission.h"
#include "Screen/InGameScreen.h"
#include "Game/Game.h"
#include "Game/Sfx.h"
#include "Map/MapFactory.h"
#include "Entity/Entity.h"
#include <cassert>

namespace nyaa {

Mission::Mission(std::string name) 
	:name(name)
	,objectiveIndex(0) 
	,failReason(FailReason_Unknown)
	,wasInited(false)
	,camSettled(false)
	,failed(false)
{

}

Mission::~Mission()
{

}

void Mission::update()
{
	if (!failed && G->winFocused)
	{
		missionTimePassed.update();
	}
	frameTick();
}

void Mission::init()
{

}

void Mission::fail()
{
	Sfx::MissionFailed->play();
	failed = true;
	failMission();
	IGS->missionFailedTextShown = true;
	IGS->missionFailedTextScaleTimer.reset();
}

void Mission::pass()
{
	failed = false;
	passMission();
	IGS->missionPassedTextShown = true;
}

void Mission::cleanupMissionEntities()
{
	for (auto m : IGS->mapFactory->maps)
	{
		for (auto e : m->entities)
		{
			if (!(e->flags & Entity::EntFlag_MissionSpawned))
			{
				continue;
			}
			e->flags |= Entity::EntFlag_Dead;
		}
	}
}

Mission* Mission::getNew()
{
	assert(0);
	return nullptr;
}

void Mission::failMission()
{

}

void Mission::passMission()
{

}

void Mission::onCameraSettle()
{

}

void Mission::frameTick()
{

}

bool Mission::onActorDeath(Actor* a)
{
	return false;
}

Mission::EventResponse Mission::onEntityInteract(Entity* e)
{
	return EventResponse_NotImplemented;
}

void Mission::onEntityTick(Entity* e)
{
}

Mission::EventResponse Mission::onDamage(float delta)
{
	return EventResponse_NotImplemented;
}

Mission::EventResponse Mission::onPropInteract(Prop* p, Actor* user)
{
	return EventResponse_NotImplemented;
}

//Mission::EventResponse Mission::onPortalEntry(Map::Portal* portal, MapTile* tile)
//{
//    return EventResponse_NotImplemented;
//}

//Mission::EventResponse Mission::onPortalActivate(Map::Portal* portal, MapTile* tile)
//{
//    return EventResponse_NotImplemented;
//}

//Mission::EventResponse Mission::onPortalLeave(Map::Portal* portal, MapTile* tile)
//{
//    return EventResponse_NotImplemented;
//}

Mission::EventResponse Mission::beforeMapSwitch(Map* curMap, Map* toMap)
{
	return EventResponse_NotImplemented;
}

Mission::EventResponse Mission::afterMapSwitch(Map* curMap, Map* lastMap)
{
	return EventResponse_NotImplemented;
}

void Mission::doScreenStatsText(std::string& stats)
{
}

}