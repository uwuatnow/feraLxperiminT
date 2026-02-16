#pragma once
#include "Screen/Screen.h"

namespace nyaa {

class MapSwitchScreen : public Screen
{
public:
	static MapSwitchScreen* Instance;

	MapSwitchScreen();

public:
	virtual bool onEnter() override;
};

}