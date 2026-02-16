#include "Screen/NewGameScreen.h"
#include "Game/Game.h"
#include "Game/Util.h"
#include "Screen/MenuScreen.h"
#include "Screen/InGameScreen.h"
#include "Mission/FirstMission.h"
#include "Game/Sfx.h"
#include "Game/Mouse.h"

namespace nyaa {

NewGameScreen* NewGameScreen::Instance = nullptr;

NewGameScreen::NewGameScreen()
	:ti(7, 10, "enter character name", "")
	,sp(nullptr)
	,skipAndLoadSave(false)
{
	auto cgb = sf::FloatRect{}, clb = sf::FloatRect{};
	ti.getCharactersGlobalBounds(&cgb.left, &cgb.top, &cgb.width, &cgb.height);
	ti.getCharactersLocalBounds(&clb.left, &clb.top, &clb.width, &clb.height);

	sp = new SelectionPrompt(Game::ScreenWidth / 2.0f, cgb.top
		+ clb.height + 25,
		150, 100, false);
	sp->add("Backspace", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		NewGameScreen::Instance->ti.backspc();
		return SelPResp_DidSomething;
	});
	
	sp->add("Finish", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		NewGameScreen::Instance->finish();
		return SelPResp_DidSomething;
	});
	
	sp->add("Nevermind", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		MenuScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
}

NewGameScreen::~NewGameScreen()
{
	delete sp;
}

void NewGameScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	ti.update();
	ti.render(renderTarget);

	auto cgb = sf::FloatRect{}, clb = sf::FloatRect{};
	ti.getCharactersGlobalBounds(&cgb.left, &cgb.top, &cgb.width, &cgb.height);
	ti.getCharactersLocalBounds(&clb.left, &clb.top, &clb.width, &clb.height);

	auto cmousepos = sf::Vector2i{ Mouse::Pos_X, Mouse::Pos_Y };
	if (Mouse::Moved)
	{
		if (cgb.contains((sf::Vector2f)cmousepos))
		{
			ti.typing = true;
			sp->update(false);
		}
		else
		{
			ti.typing = false;
		}
	}
	if (!ti.typing)
	{
		sp->update();
		if (sp->upFail)
		{
			ti.typing = true;
			sp->update(false);
			Sfx::CursorMove->play();
		}
	}
	else
	{
		sp->selectedIndex = 0;
		//for(auto& s : sp.selectionVec) s.animTimer.zero();
	}

// #if DEBUG
// 	finish();
// #endif

	sp->render(renderTarget);
}

bool NewGameScreen::onEnter()
{
	G->ReallocInGameScreen();

	if (skipAndLoadSave)
	{
		skipAndLoadSave = false;
		finish(true);
		return true;
	}
#if DEBUG
	ti.setName(Util::Format("enter character name [slot %d]", G->selectedSaveSlot + 1));
#endif
	return true;
}

bool NewGameScreen::onLeave()
{
	ti.reset();
	sp->selectedIndex = 0;
	sp->update();
	//nevermind = false;
	return true;
}

void NewGameScreen::finish(bool force)
{
	if (G->fadeState != Fade_Done && !force) return;
// #if DEBUG
// 	ti.input.setString("Debug");
// #endif

	// -> saves will be loaded here!!!

	//todo: reset more shit here
	
	//this for now
	IGS->setMission(new FirstMission());
	// inst->igs->saveData.reset();
	//inst->igs->saveData.j["playerName"] = ti.input.getString();
	IGS->switchTo(force);
}

}