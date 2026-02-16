#include "Mission/IdleMission.h"

namespace nyaa {

IdleMission::IdleMission()
	:Mission("idle")
{
	
}

IdleMission::~IdleMission()
{
	
}

void IdleMission::doScreenStatsText(std::string& stats)
{
	//stats += "\n" "Go to bed";
}

}