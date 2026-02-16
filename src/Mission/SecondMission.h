#pragma once
#include "Game/Globals.h"
#include "Mission.h"

namespace nyaa {

class SecondMission : public Mission
{
public:
	SecondMission();

	virtual ~SecondMission();

public:
	virtual void onEntityTick(Entity* e) override;

	virtual void init() override;

	virtual void onCameraSettle() override;

	virtual void failMission() override;

	virtual void passMission() override;

	virtual bool onActorDeath(Actor* a) override;

	EventResponse onPropInteract(Prop* p, Actor* user) override;

	virtual Mission* getNew() override;

	virtual void doScreenStatsText(std::string& stats) override;

private:
};

}
