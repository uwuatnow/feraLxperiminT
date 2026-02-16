#pragma once
#include "Game/Globals.h"

#include <vector>
#include <functional>

namespace nyaa {

class EventScheduler
{
public:
	static EventScheduler* GlobalScheduler;

	typedef std::function<void()> SchFunc;

	struct SchEvent {
		float timeAhead;
		SchFunc func;
	};

public:
	void scheduleEvent(float timeAhead, SchFunc func);
	void update(float deltaMillis);

private:
	std::vector<SchEvent> scheduledEvents;
};

}