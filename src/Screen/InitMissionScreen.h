#pragma once
#include "Game/Globals.h"
#include "Screen/Screen.h"

namespace nyaa {

class InitMissionScreen : public Screen
{
public:
	static InitMissionScreen* Instance;

	InitMissionScreen();

	virtual void doTick(RendTarget* renderTarget) override;
	
	static void DoInit();

public:
	static Screen* OverrideReturnScreen;
};

}