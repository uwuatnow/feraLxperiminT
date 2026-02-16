#pragma once
#include "Screen/Screen.h"
#include "Game/SelectionPrompt.h"
#include <SFML/Graphics/Text.hpp>

namespace nyaa {

class MissionSelectDebugScreen : public Screen
{
public:
	MissionSelectDebugScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;

	virtual bool onEnter() override;

public:
	SelectionPrompt sp;
	sf::Text mText;
};

extern MissionSelectDebugScreen* missionSelectScreen;
}