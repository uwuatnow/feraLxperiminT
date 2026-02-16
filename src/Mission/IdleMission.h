#pragma once
#include "Game/Globals.h"
#include "Mission.h"

namespace nyaa {

class IdleMission : public Mission
{
public:
	IdleMission();

	virtual ~IdleMission();
	
public:
	
	virtual void doScreenStatsText(std::string& stats) override;
};

}