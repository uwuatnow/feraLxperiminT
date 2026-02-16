#pragma once
#include "Game/Globals.h"
#include "Mission/Mission.h"

namespace nyaa {

class DevTestMission : public Mission
{
public:
	DevTestMission();

	virtual ~DevTestMission();

public:

	virtual void init() override;
};

}