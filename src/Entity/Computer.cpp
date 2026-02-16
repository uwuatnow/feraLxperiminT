#include "Entity/Computer.h"
#include "Screen/InGameScreen.h"
#include "Game/Util.h"
#include "Entity/Interaction.h"
#include "Entity/Actor.h"
#include "Mission/Mission.h"
#include "Screen/PCScreen.h"
#include "Game/EventScheduler.h"
#include "Screen/PCSite.h"
#include "Game/Timer.h"

namespace nyaa {

Computer::Computer()
	:Prop("computer", 2, 2, Direction_Center, true)
{
	state = State_Off;
	bootTimer = nullptr;
	friction = 2.0f;
	interactions.push_back(new Interaction(this, "Use computer", [this](Interaction* in) -> bool
	{
		auto& db = IGS->dbox;
		if (IGS->curMission->onEntityInteract(in->e) == Mission::EventResponse_NotImplemented)
		{
			if (state == State_Off)
			{
				state = State_Booting;
				bootTimer = new Timer(0.0f); // start at 0, count up to 10 seconds
			}
			PCScreen::Instance->accessedFrom = this;
			PCScreen::Instance->switchTo();
		}
		return true;
	}));
	dirAngle = 90;

	icons.push_back({ "My Computer", 60, 60, false, 0, 0, -1.0f });
	icons.push_back({ "Trash", 60, 160, false, 0, 0, -1.0f });
	icons.push_back({ "Notes", 60, 260, false, 0, 0, -1.0f });
	icons.push_back({ "Internet", 60, 360, false, 0, 0, -1.0f });
	icons.push_back({ "Forum", 60, 460, false, 0, 0, -1.0f });
	icons.push_back({ "Site Index", 160, 60, false, 0, 0, -1.0f });
	icons.push_back({ "Search Engine", 160, 160, false, 0, 0, -1.0f });
	icons.push_back({ "Shutdown", 160, 260, false, 0, 0, -1.0f });

	// Initialize random number generator for window position variation
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(-50.0f, 50.0f);
	windowPosRandomGen = gen;
	windowPosRandomDist = dist;
}

void Computer::shutdown()
{
	// Clean up site pointers
	for (auto& win : windows)
	{
		if (win.site) delete win.site;
	}
	windows.clear();

	// Reset state
	state = State_Off;
	
	if (bootTimer)
	{
		delete bootTimer;
		bootTimer = nullptr;
	}
}

void Computer::update()
{
	Entity::update(); // call base
}

bool Computer::onInteract(Actor* user)
{
	return true;
}

}