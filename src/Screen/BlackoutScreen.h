#pragma once
#include "Game/Globals.h"
#include "Screen/Screen.h"

namespace nyaa {

class BlackoutScreen : public Screen
{
public:
	static BlackoutScreen* Instance;

	BlackoutScreen();
	
	virtual ~BlackoutScreen();
	
public:
	virtual bool onEnter() override;

public:
	//std::shared_ptr<sol::function> callback;
};

}