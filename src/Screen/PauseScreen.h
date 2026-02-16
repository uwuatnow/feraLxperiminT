#pragma once
#include "Screen/Screen.h"
#include "Game/SelectionPrompt.h"

namespace nyaa {

class PauseScreen : public Screen
{
public:
	static PauseScreen* Instance;

	PauseScreen();
	
	virtual ~PauseScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;
	
	virtual bool onEnter() override;

public:
	SelectionPrompt sp;
};

}