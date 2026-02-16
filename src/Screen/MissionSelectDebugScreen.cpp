#include "Screen/MissionSelectDebugScreen.h"
#include "Screen/InitMissionScreen.h"
#include "Mission/FirstMission.h"
#include "Mission/SecondMission.h"
#include "Game/Game.h"
#include "Screen/InGameScreen.h"
#include "Screen/MenuScreen.h"
#include "Mission/KillEvilCatgirlsMission.h"
#include "Mission/IdleMission.h"
#include "Mission/DevTestMission.h"
#include "Game/Fonts.h"

namespace nyaa {

MissionSelectDebugScreen* missionSelectScreen = nullptr;

MissionSelectDebugScreen::MissionSelectDebugScreen()
	:sp(Game::ScreenWidth / 2, Game::ScreenHeight / 2, 150, 200)
	,mText("- debug Mission select -", *Fonts::OSDFont)
{
	sp.add("<- back 2 main menu.", [](SelectionPrompt& sp, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		MenuScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	sp.add("[Idle]", [](SelectionPrompt& sp, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->setMission(new IdleMission());
		InitMissionScreen::OverrideReturnScreen = IGS;
		InitMissionScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	sp.selectedIndex = 1;
	sp.add("Out of the basement", [](SelectionPrompt& sp, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->setMission(new FirstMission());
		InitMissionScreen::OverrideReturnScreen = IGS;
		InitMissionScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	sp.add("2nd Mission", [](SelectionPrompt& sp, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->setMission(new SecondMission());
		InitMissionScreen::OverrideReturnScreen = IGS;
		InitMissionScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	
	sp.add("Kill Evil catgirls", [](SelectionPrompt& sp, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->setMission(new KillEvilCatgirlsMission());
		InitMissionScreen::OverrideReturnScreen = IGS;
		InitMissionScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});

	sp.add("DevTest 0001", [](SelectionPrompt& sp, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if (d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->setMission(new DevTestMission());
		InitMissionScreen::OverrideReturnScreen = IGS;
		InitMissionScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
}

void MissionSelectDebugScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	if(Kb::IsKeyReleased(KB::Escape))
	{
		MenuScreen::Instance->switchTo();
	}
	sp.update();
	sp.render(renderTarget);
	renderTarget->draw(mText);
}

bool MissionSelectDebugScreen::onEnter()
{
	G->ReallocInGameScreen();
	return true;
}

}
