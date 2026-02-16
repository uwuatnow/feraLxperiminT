#pragma once
#include "Screen/Screen.h"

namespace nyaa {

class MissionFailedScreen : public Screen
{
public:
	static MissionFailedScreen* Instance;

	MissionFailedScreen();

	virtual void doTick(RendTarget* renderTarget) override;
};

}