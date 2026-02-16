#pragma once
#include "Game/Globals.h"

#include <string>
#include "Map/Map.h"
#include "Game/Timer.h"

namespace nyaa {

class Mission
{
public:
	enum FailReason
	{
		FailReason_Unknown,
		FailReason_Died,
		FailReason_KilledUrMom
	};

	enum EventResponse
	{
		EventResponse_Prevent, 
		EventResponse_Allow, 
		EventResponse_NotImplemented
	};

public:
	Mission(std::string name);
	
	virtual ~Mission();

public:
	void update(); /* runs every frame while mission is active*/
	
	void fail();
	
	void pass();
	
	void cleanupMissionEntities();
	
	virtual Mission* getNew();
	
	virtual void init();
	
	virtual void onCameraSettle();
	
	//append to stats and it shows up on top left in game
	virtual void doScreenStatsText(std::string& stats);

protected:
	virtual void failMission();
	
	virtual void passMission();

public:
	virtual void frameTick();
	
	virtual bool onActorDeath(class Actor *a); //true fails mission
	
	virtual EventResponse onEntityInteract(class Entity* e); /* when main character interacts with an entity */
	
	virtual void onEntityTick(class Entity* e);//called by all entities during mission
	
	virtual EventResponse onDamage(float delta);
	
	virtual EventResponse onPropInteract(class Prop *p, class Actor *user);
	
	virtual EventResponse beforeMapSwitch(class Map* curMap, class Map* toMap); /* before switching to another map */
	
	virtual EventResponse afterMapSwitch(class Map* curMap, class Map* lastMap); /* after map switch complete */
	
public:
	std::string name;
	Timer missionTimePassed;
	unsigned int objectiveIndex;
	FailReason failReason;
	bool wasInited;
	bool camSettled;
	bool failed;
};


}