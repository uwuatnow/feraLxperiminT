#pragma once
#include "Game/Globals.h"
#include "Mission/Mission.h"

namespace nyaa {

class FirstMission : public Mission
{
public:
	FirstMission();

	virtual ~FirstMission();

public:
	virtual void onEntityTick(class Entity* e) override;

	virtual void init() override;

	virtual void onCameraSettle() override;

	virtual void failMission() override;

	virtual void passMission() override;

	virtual bool onActorDeath(class Actor* a) override;

	EventResponse onPropInteract(class Prop* p, class Actor* user) override;

	virtual Mission* getNew() override;

	virtual void doScreenStatsText(std::string& stats) override;

private:
	class Catgirl* mom;
	class Microwave* microwave;
	bool microwaveUsed;
	//class Item* cookingHotPocket;
	bool pottyUsed;
	
private:
	friend class Potty;
};

}
