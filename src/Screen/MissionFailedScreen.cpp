#include "Screen/MissionFailedScreen.h"
#include "Game/Game.h"
#include "Screen/InitMissionScreen.h"
#include "Screen/InGameScreen.h"
#include "Mission/Mission.h"

namespace nyaa {

MissionFailedScreen* MissionFailedScreen::Instance = nullptr;

MissionFailedScreen::MissionFailedScreen()
{
	Instance = this;
}

void MissionFailedScreen::doTick(RendTarget* renderTarget) {
	Screen::doTick(renderTarget);
	if (G->fadeState == Fade_Done) {
		IGS->setMission(IGS->curMission->getNew());
		InitMissionScreen::OverrideReturnScreen = IGS;
		InitMissionScreen::Instance->switchTo();
		IGS->curMission->cleanupMissionEntities();
	}
}

}