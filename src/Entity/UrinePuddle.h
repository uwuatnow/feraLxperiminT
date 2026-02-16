#pragma once
#include "Game/Globals.h"
#include "Entity/Entity.h"

namespace nyaa {

class UrinePuddle : public Entity
{
public:
	static float MinimumPissTime;

public:	
	UrinePuddle(class Actor* wetter);
	virtual ~UrinePuddle();
	
	void update() override;

public:
	int PuddleRadiusPx;
	class Timer* puddleSpreadTimer;
	class Actor* wetter;
	bool done;
	float seconds;
	float bladderOnPiss;
};

}