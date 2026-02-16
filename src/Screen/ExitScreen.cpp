#include "Screen/ExitScreen.h"
#include "Game/Game.h"

namespace nyaa {

ExitScreen* ExitScreen::Instance = nullptr;

ExitScreen::ExitScreen()
{
	Instance = this;
}

ExitScreen::~ExitScreen()
{
}

void ExitScreen::doTick(RendTarget* renderTarget)
{
	//TODO: maybe have an exit confirmation later on to prevent the player from closing the game if they progressed without saving
	G->win->close();
}

}