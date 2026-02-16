#pragma once
#include "Screen/Screen.h"
#if DEBUG

namespace nyaa {

class TileSelectScreen : public Screen
{
public:
	TileSelectScreen();
	
	virtual ~TileSelectScreen();
	
public:
	virtual void doTick(RendTarget* renderTarget) override;
	
	virtual bool onEnter() override;
};

extern TileSelectScreen* tileSelectScreen;
}
#endif