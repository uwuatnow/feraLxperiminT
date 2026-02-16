#include "Screen/InitMissionScreen.h"
#include "Game/Game.h"
#include "Screen/InGameScreen.h"
#include "Mission/Mission.h"
#include <cassert>

namespace nyaa {

InitMissionScreen* InitMissionScreen::Instance = nullptr;
Screen* InitMissionScreen::OverrideReturnScreen = nullptr;

InitMissionScreen::InitMissionScreen()
{
	Instance = this;
}

void InitMissionScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	if (G->fadeState == Fade_Done) {
		DoInit();
		if (OverrideReturnScreen) {
			OverrideReturnScreen->switchTo();
			OverrideReturnScreen = nullptr;
		}
		else Screen::LastScreen->switchTo();
	}
}

void InitMissionScreen::DoInit()
{
	assert(IGS->curMission);
	IGS->curMission->init();
	IGS->curMission->wasInited = true;
}

}