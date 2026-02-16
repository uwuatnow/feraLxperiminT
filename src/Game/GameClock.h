#pragma once
#include "Game/Globals.h"

#include <string>
#include "Game/Timer.h"

namespace nyaa {

enum class Weekday {
	Sunday = 0,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
};

class GameClock
{
public:
	static int BaseYear;
	
public:
	GameClock();

public:
	void tick();

	void AddMinutes(int count);

	Weekday getWeekday() const;
	
	float getTimeOfDay() const;

public:
	std::string	timeStr;

private:
	int year;
	int month;
	int day;
	int hour;
	int minute;
	
	Timer timer;
};

}
