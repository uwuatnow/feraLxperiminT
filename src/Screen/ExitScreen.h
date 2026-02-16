#pragma once
#include "Game/Globals.h"
#include "Screen/Screen.h"

namespace nyaa {

class ExitScreen : public Screen
{
public:
	static ExitScreen* Instance;

	ExitScreen();

	virtual ~ExitScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;
};

}