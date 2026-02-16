#include "Game/GameClock.h"
#include "Game/Util.h"
#include "Game/Game.h"

namespace nyaa {

int GameClock::BaseYear = 2021;

constexpr const char* WeekdayNames[] = {
	"Sun",
	"Mon", 
	"Tues",
	"Wed",
	"Thur",
	"Fri",
	"Sat"
};

GameClock::GameClock()
	:year(BaseYear)
	,month(3)
	,day(1)
	,hour(13)
	,minute(56)
{
	
}

Weekday GameClock::getWeekday() const
{
	int totalDays = (year - BaseYear) * 365 + month * 30 + day;
	return static_cast<Weekday>(totalDays % 7);
}

float GameClock::getTimeOfDay() const
{
	return (hour * 60.0f + minute) / (24.0f * 60.0f);
}

void GameClock::tick()
{
	if(G->winFocused) timer.update();
	if(timer.once(2220))
	{
		AddMinutes(1);
	}
	Weekday weekday = getWeekday();
	timeStr = Util::Format(
	  "%s\n"
	  "%d/%d/%d\n"
	  "%02d" ":" "%02d" " %s"
	  ,WeekdayNames[static_cast<int>(weekday)]
	  ,month + 1
	  ,day + 1
	  ,year
	  ,hour == 0 ?
		  12
		  : hour > 12 ?
		      hour - 12
		      : hour
	  ,minute
	  ,hour > 11 ?
	      "PM"
	      : "AM"
	);
}

void GameClock::AddMinutes(int count)
{
	minute += count;
	while(minute >= 60)
	{
		minute -= 60;
		hour++;
	}
	while(hour >= 24)
	{
		hour -= 24;
		day++;
	}
	while(day >= 30)
	{
		day -= 30;
		month++;
	}
	while(month >= 12)
	{
		month -= 12;
		year++;
	}
}

}
