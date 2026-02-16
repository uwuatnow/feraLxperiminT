#include "Screen/EditTextScreen.h"
#include "Game/Game.h"
#include "Game/Sfx.h"
#include <cassert>
#include "Game/Mouse.h"

namespace nyaa {

EditTextScreen* EditTextScreen::Instance = nullptr;

EditTextScreen::EditTextScreen()
	:str(nullptr)
	,ti(5, 5, "enter text now", "")
	,sp(nullptr)
{
	Instance = this;
	float cgt = 0, clh = 0;
	ti.getCharactersGlobalBounds(nullptr, &cgt, nullptr, nullptr);
	ti.getCharactersLocalBounds(nullptr, nullptr, nullptr, &clh);
	sp = new SelectionPrompt((float) Game::ScreenWidth / 2,
		cgt + clh + 25,
		150,
		100
	);
	sp->add("Backspace", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		EditTextScreen::Instance->ti.backspc();
		return SelPResp_DidSomething;
	});
	
	sp->add("Done", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		if (EditTextScreen::Instance->myLastScreen)
			EditTextScreen::Instance->myLastScreen->switchTo();
		return SelPResp_DidSomething;
	});
}

EditTextScreen::~EditTextScreen()
{
	delete sp;
}

void EditTextScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	ti.update();
	ti.render(renderTarget);

	auto cmousepos = sf::Vector2i{ Mouse::Pos_X, Mouse::Pos_Y };
	if (Mouse::Moved)
	{
		auto cgb = sf::FloatRect{};
		ti.getCharactersGlobalBounds(&cgb.left, &cgb.top, &cgb.width, &cgb.height);
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
	}

	sp->render(renderTarget);
}

bool EditTextScreen::onEnter()
{
	assert(str);
	if (str) {
		ti.setNameOrigin();
		ti.reset();
		ti.input.setString(*str);
		return true;
	}
	return false;
}

bool EditTextScreen::onLeave()
{
	if (str) {
		(*str) = ti.input.getString();
	}
	return true;
}

}