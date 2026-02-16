#include "Screen/Screen.h"
#include "Game/Game.h"
#include "Game/GuiWidget.h"

namespace nyaa {

Screen* Screen::CurrentScreen = nullptr;
Screen* Screen::ToScreen = nullptr;
Screen* Screen::LastScreen = nullptr;

Screen::Screen()
	:myLastScreen(nullptr)
{
}

Screen::~Screen()
{
}

void Screen::doTick(RendTarget* renderTarget)
{
	guip = this;
}

void Screen::switchTo(bool noFadeTamper)
{
	myLastScreen = Screen::CurrentScreen;
	Screen::ToScreen = this;
	if (!noFadeTamper)
	{
		G->fadeState = Fade_Out;
		G->screenSwitchClock.restart();
	}
}

bool Screen::onEnter() 
{
	return true;
}

bool Screen::onLeave() 
{
	return true;
}

}