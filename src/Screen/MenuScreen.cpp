#include "Screen/MenuScreen.h"
#include "Game/Game.h"
#include "Screen/NewGameScreen.h"
#include "Screen/OptionsScreen.h"
#include "Screen/LoadGameScreen.h"
#include "Screen/MissionSelectDebugScreen.h"
#include "Game/Easing.h"
#include <cmath>
#include "Game/Fonts.h"

namespace nyaa {

MenuScreen* MenuScreen::Instance = nullptr;

MenuScreen::MenuScreen()
	:sp(320, 180, 100, 200)
	,showCredits(false)
{
#if DEBUG
	sp.add("Mission select", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		missionSelectScreen->switchTo();
		return SelPResp_DidSomething;
	});
#endif
	sp.add("New file", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		//NewGameScreen::Instance->switchTo();
		LoadGameScreen::Instance->overwriteWithNewFileModeOn = true;
		LoadGameScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	sp.add("Load", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		LoadGameScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	sp.add("options", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		OptionsScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	sp.add("EXIT", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		G->close();
		return SelPResp_DidSomething;
	});
	//
	sp.add("CREDITS", [this](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		this->showCredits ^= true;
		return SelPResp_DidSomething;
	});
}

void MenuScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);

	if(Kb::IsKeyFirstFrame(KB::Escape))
	{
		G->close();
	}

	sp.update();
#if DEBUG
	if (G->framesPassed == 0) NewGameScreen::Instance->switchTo();
#endif

	sp.render(renderTarget);

	if(showCredits) {
		// Animated credits background
		sf::RectangleShape creditsBg(sf::Vector2f(Game::ScreenWidth * 0.8f, Game::ScreenHeight * 0.8f));
		creditsBg.setPosition(Game::ScreenWidth * 0.1f, Game::ScreenHeight * 0.1f);
		creditsBg.setFillColor(sf::Color(0, 0, 0, 180));
		creditsBg.setOutlineThickness(3);
		creditsBg.setOutlineColor(sf::Color::Cyan);
		renderTarget->draw(creditsBg);
		
		sf::Text credits(
			"Credits:\n"
			"\n"
			"CarDoorOpenClose4.wav by phenoxy\n"
			"https://freesound.org/s/195450/\n"
			"License: Attribution 4.0\n"
			"\n"
			"Gear shifts and other noises\n"
			"Toyota Verso Interior by JaimeLopes\n"
			"https://freesound.org/s/442766/\n"
			"License: Attribution 4.0\n"
			"\n"
			"-- END --\n"
			, 
			*Fonts::MainFont, 
			20
		);
		credits.setPosition(Game::ScreenWidth * 0.15f, Game::ScreenHeight * 0.15f);
		credits.setFillColor(sf::Color::White);
		credits.setOutlineColor(sf::Color::Cyan);
		credits.setOutlineThickness(1);
		renderTarget->draw(credits);
	}
}

bool MenuScreen::onEnter()
{
	showCredits = false;
	return true;
}

}