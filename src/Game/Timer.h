#pragma once
#include "Game/Globals.h"

#include "Game/Easing.h"
#include "Game/Clock.h"

namespace nyaa {

class Timer
{
public:
	Timer();
	
	Timer(float startMilliseconds, float restartThresholdMs = 100, 
		bool resetOnFirstUse = true, float lengthMs = 0, Ease ease = Ease_None
	);

public:
	void update();
	
	void update2(bool minus, long long int msLimit = 0); //0 is no limit
	
	void reset(float startMilliseconds = 0);
	
	float secs();
	
	float millis();
	
	void setMillis(float milliseconds);
	
	void zero();
	
	bool once(float milliseconds); //return true once timer is above threshold then reset

public:
	long long microseconds;
	unsigned long long int updateFrame;
	float restartThresholdMs; // cancel update if the clock ms is >= this number
	bool resetOnFirstUse;
	float lengthMs;
	Ease ease;
	Clock clock;
};

}