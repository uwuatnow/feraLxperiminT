#include "Game/EventScheduler.h"

namespace nyaa {

EventScheduler* EventScheduler::GlobalScheduler = nullptr;

void EventScheduler::scheduleEvent(float timeAhead, SchFunc func)
{
	scheduledEvents.push_back({timeAhead, func});
}

void EventScheduler::update(float deltaMillis)
{
	for(auto it = scheduledEvents.begin(); it != scheduledEvents.end(); )
	{
		auto& sev = *it;
		sev.timeAhead -= deltaMillis;
		if(sev.timeAhead < 0) {
			sev.func();
			it = scheduledEvents.erase(it);
		} else it++;
	}
}

}