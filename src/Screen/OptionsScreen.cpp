#include "Screen/OptionsScreen.h"
#include "Game/Game.h"
#include "Game/GameSettings.h"
#include "Game/Controller.h"

namespace nyaa {

OptionsScreen* OptionsScreen::Instance = nullptr;

OptionsScreen::OptionsScreen()
	:sp(120, 300, 100, 100)
{
	Instance = this;

	// Add the Back button
	sp.add("Back", [this](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if (d != Direction_Center)
			return SelPResp_Neutral;
		if (myLastScreen)
			myLastScreen->switchTo();
		return SelPResp_DidSomething;
	});
}

OptionsScreen::~OptionsScreen()
{
}

void OptionsScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	// Handle Escape/Q/Controller as before
	if (
		(
			Kb::IsKeyReleased(KB::Escape)
			|| Kb::IsKeyReleased(KB::Q)
			|| Controller::BtnFrames[Btn_Circle] == 1
		)
		&& myLastScreen
	)
	{
		myLastScreen->switchTo();
	}

	// Update and render the prompt
	sp.update();
	sp.render(renderTarget);
}

}
