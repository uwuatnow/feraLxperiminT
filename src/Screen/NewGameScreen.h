#pragma once
#include "Screen/Screen.h"
#include "Game/SelectionPrompt.h"
#include "Text/TextInput.h"

namespace nyaa {

class NewGameScreen : public Screen
{
public:
	NewGameScreen();
	
	virtual ~NewGameScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;
	
	virtual bool onEnter() override;
	
	virtual bool onLeave() override;
	
	void finish(bool force = false);

public:
	TextInput ti;
	SelectionPrompt* sp;
	bool skipAndLoadSave;

	static NewGameScreen* Instance;
};

}