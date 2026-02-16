#pragma once
#include "Screen/Screen.h"

namespace nyaa {

class SelectionPrompt;

class LoadGameScreen : public Screen
{
public:
	static LoadGameScreen* Instance;

    LoadGameScreen(); 

    ~LoadGameScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;
	
	virtual bool onEnter() override;

    virtual bool onLeave() override;

public:
    bool overwriteWithNewFileModeOn;
    SelectionPrompt* selPrompt;
};

}