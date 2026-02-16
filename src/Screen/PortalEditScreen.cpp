#include "Screen/PortalEditScreen.h"
#include "Game/Game.h"
#include <cassert>
#include "Screen/EditTextScreen.h"
#include "Map/Map.h"
#include "Screen/InGameScreen.h"

namespace nyaa {

PortalEditScreen* PortalEditScreen::Instance = nullptr;

PortalEditScreen::PortalEditScreen()
	:sp(Game::ScreenWidth / 2.0f, 30, 100, Game::ScreenHeight)
	,editingPortal(nullptr)
	,ofdSel(sp.add("Out face dir: 0", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			if(d == Direction_Left)
			{
				PortalEditScreen::Instance->editingPortal->outDirAngleDeg -= 10;
			}
			else if(d == Direction_Right)
			{
				PortalEditScreen::Instance->editingPortal->outDirAngleDeg += 10;
			}
			s.text.setString("Out face dir: " + std::to_string(PortalEditScreen::Instance->editingPortal->outDirAngleDeg));
			return SelPResp_DidSomething;
		}
		return SelPResp_Neutral;
	}))
	,owdSel(sp.add("Out walk distance: 0", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			if(d == Direction_Left)
			{
				PortalEditScreen::Instance->editingPortal->outWalkAmount -= 1;
			}
			else if(d == Direction_Right)
			{
				PortalEditScreen::Instance->editingPortal->outWalkAmount += 1;
			}
			s.text.setString("Out walk distance: " + std::to_string(PortalEditScreen::Instance->editingPortal->outWalkAmount));
			return SelPResp_DidSomething;
		}
		return SelPResp_Neutral;
	}))

{
	Instance = this;

	sp.add("Edit portal name", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		assert(PortalEditScreen::Instance->editingPortal);
		EditTextScreen::Instance->ti.setName("Portal name");
		EditTextScreen::Instance->str = &PortalEditScreen::Instance->editingPortal->name;
		EditTextScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	
	sp.add("Edit destination map name", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		assert(PortalEditScreen::Instance->editingPortal);
		EditTextScreen::Instance->ti.setName("Destination map name");
		EditTextScreen::Instance->str = &PortalEditScreen::Instance->editingPortal->destMapName;
		EditTextScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	
	sp.add("Edit destination portal name", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		assert(PortalEditScreen::Instance->editingPortal);
		EditTextScreen::Instance->ti.setName("Destination portal name");
		EditTextScreen::Instance->str = &PortalEditScreen::Instance->editingPortal->destPortalName;
		EditTextScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});
	
	tileXSel = &sp.add("Tile X: 0", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			if(d == Direction_Left)
			{
				PortalEditScreen::Instance->editingPortal->tilePosX--;
				return SelPResp_DidSomething;
			}
			else if(d == Direction_Right)
			{
				PortalEditScreen::Instance->editingPortal->tilePosX++;
				return SelPResp_DidSomething;
			}
			s.text.setString("Tile X: " + std::to_string(PortalEditScreen::Instance->editingPortal->tilePosX));
		}
		return SelPResp_Neutral;
	});
	
	tileYSel = &sp.add("Tile Y: 0", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			if(d == Direction_Left)
			{
				PortalEditScreen::Instance->editingPortal->tilePosY--;
			}
			else if(d == Direction_Right)
			{
				PortalEditScreen::Instance->editingPortal->tilePosY++;
			}
			s.text.setString("Tile Y: " + std::to_string(PortalEditScreen::Instance->editingPortal->tilePosY));
			return SelPResp_DidSomething;
		}
		return SelPResp_Neutral;
	});
	
	sp.add("Done", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		if (PortalEditScreen::Instance->myLastScreen)
		{
			IGS->switchTo();
		}
		return SelPResp_DidSomething;
	});
}

PortalEditScreen::~PortalEditScreen()
{
	
}

void PortalEditScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	sp.update();
	sp.render(renderTarget);
}

bool PortalEditScreen::onEnter()
{
	ofdSel.text.setString("Out face dir: " + std::to_string(editingPortal->outDirAngleDeg));
	owdSel.text.setString("Out walk distance: " + std::to_string(editingPortal->outWalkAmount));
	tileXSel->text.setString("Tile X: " + std::to_string(editingPortal->tilePosX));
	tileYSel->text.setString("Tile Y: " + std::to_string(editingPortal->tilePosY));
	return true;
}

}