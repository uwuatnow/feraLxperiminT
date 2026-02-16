#pragma once
#include "Screen/Screen.h"
#include "Game/SelectionPrompt.h"
#include "Game/GameSettings.h"

namespace nyaa {

class OptionsScreen : public Screen
{
public:
	static OptionsScreen* Instance;

	OptionsScreen();
	virtual ~OptionsScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;

private:
	SelectionPrompt sp;
};

}