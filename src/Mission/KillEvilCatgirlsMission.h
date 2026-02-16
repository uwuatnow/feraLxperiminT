#pragma once
#include "Game/Globals.h"
#include "Mission.h"

namespace nyaa {

class KillEvilCatgirlsMission : public Mission 
{
public:
	KillEvilCatgirlsMission();

public:
	virtual void init() override;
	
	virtual bool onActorDeath(Actor* a) override;
	
	virtual void doScreenStatsText(std::string& stats) override;
};

}