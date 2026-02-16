#pragma once
#include "Game/Globals.h"
#include "Game/Timer.h"

namespace nyaa {

class RainDrop
{
public:	
	RainDrop();
	
public:
	float x, y;
	float dirHoriz; //-1.0 - 1.0
	unsigned char frame; //0-3
	Timer timer;
	Clock life;
};

}