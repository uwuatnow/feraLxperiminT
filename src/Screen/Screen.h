#pragma once
#include "Game/Globals.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include "Game/GuiWidget.h"

namespace nyaa {

class Screen : public GuiWidget
{
public:
	Screen();
	
	virtual ~Screen();

public:
	static Screen* CurrentScreen;
	static Screen* ToScreen;
	static Screen* LastScreen;
	
public:
	virtual void doTick(RendTarget* renderTarget);

	virtual void switchTo(bool noFadeTamper = false);

	//returns true to allow switch
	virtual bool onEnter();
	
	virtual bool onLeave();
	
public:
	Screen* myLastScreen;
};


}