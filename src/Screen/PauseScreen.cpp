#include "Screen/PauseScreen.h"
#include "Game/Game.h"
#include "Screen/InGameScreen.h"
#include "Screen/MenuScreen.h"
#include "Screen/TileSelectScreen.h"
#include "Game/SaveData.h"
#include "Game/Controller.h"
#include "Map/New3DRenderer.h"
#include "Entity/Actor.h"
#include <algorithm>

namespace nyaa {

/*
	NOTE: When adding to the selection prompt in this screen (this->sp),
	remember to update the debug selections text in the PauseScreen::onEnter() method.
*/

PauseScreen* PauseScreen::Instance = nullptr;

PauseScreen::PauseScreen()
	:sp(Game::ScreenWidth / 2, 100, 150, Game::ScreenHeight * 0.5)
{
	Instance = this;

	sp.add("Back to game (ESC)", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->switchTo();
		return SelPResp_DidSomething;
	});

	sp.add("Save your progress!", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->saveData->save();
		IGS->switchTo();
		return SelPResp_DidSomething;
	});

	sp.add("Save & quit to menu", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->saveData->save();
		MenuScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});

#if DEBUG
	sp.add("Tile select", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		tileSelectScreen->switchTo();
		return SelPResp_DidSomething;
	});
#define MAP_LAYER_STR std::string("Map layer: ") + (IGS->selectedLayer == 0 ? "Floor" : IGS->selectedLayer == 1 ? "Entity" : IGS->selectedLayer == 2 ? "Ceiling" : "Unknown")
	sp.add(MAP_LAYER_STR, [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if (d == Direction_Center) d = Direction_Right;
		if (d == Direction_Left)
		{
			if (IGS->selectedLayer > 0) IGS->selectedLayer--;
			else IGS->selectedLayer = 2;
		}
		else if (d == Direction_Right)
		{
			if (IGS->selectedLayer < 2) IGS->selectedLayer++;
			else IGS->selectedLayer = 0;
		}
		s.setText(MAP_LAYER_STR);
		return SelPResp_DidSomething;
	});

	sp.add("Editing tiles: " + std::string(IGS->tileEditOn ? "Yes" : "No"), [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->tileEditOn = !IGS->tileEditOn;
		s.setText("Editing tiles: " + std::string(IGS->tileEditOn ? "Yes" : "No"));
		return SelPResp_DidSomething;
	});
	
	sp.add("Editing walls: " + std::string(IGS->collisionDebug ? "Yes" : "No"), [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->collisionDebug = !IGS->collisionDebug;
		s.setText("Editing walls: " + std::string(IGS->collisionDebug ? "Yes" : "No"));
		return SelPResp_DidSomething;
	});
	
	sp.add("Show hitboxes: " + std::string(IGS->showHitboxes ? "Yes" : "No"), [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->showHitboxes = !IGS->showHitboxes;
		s.setText("Show hitboxes: " + std::string(IGS->showHitboxes ? "Yes" : "No"));
		return SelPResp_DidSomething;
	});
	
	sp.add("Raining: " + std::string(IGS->raining ? "Yes" : "No"), [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->raining = !IGS->raining;
		if (!IGS->raining) IGS->RainDrops.clear();
		s.setText("Raining: " + std::string(IGS->raining ? "Yes" : "No"));
		return SelPResp_DidSomething;
	});
	
	sp.add("Walk thru walls: " + std::string(IGS->collisionDisabled ? "Yes" : "No"), [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->collisionDisabled = !IGS->collisionDisabled;
		s.setText("Walk thru walls: " + std::string(IGS->collisionDisabled ? "Yes" : "No"));
		return SelPResp_DidSomething;
	});
	
	sp.add("Heal player", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		IGS->player->healCompletely();
		return SelPResp_DidSomething;
	});
#endif
}

PauseScreen::~PauseScreen()
{

}

void PauseScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	sp.update();
	sp.render(renderTarget);

	if (Kb::IsKeyReleased(KB::Escape) 
		|| Controller::BtnFrames[Btn_Circle] == 1 
		|| Kb::IsKeyReleased(KB::Q)
	) 
	{
		IGS->switchTo();
	}
}

bool PauseScreen::onEnter()
{
	sp.selectedIndex = 0;
	
	// Update debug selection texts
	New3DRenderer* renderer3D = dynamic_cast<New3DRenderer*>(IGS->renderer);
	if (renderer3D) {
		for (auto& selection : sp.selections) {
			std::string text = selection.text.getString().toAnsiString();
			if (text.find("Wavy drug effect:") != std::string::npos) {
				selection.setText("Wavy drug effect: " + std::to_string((int)(renderer3D->getCurrentWavyWorldEffect() * 100)) + "%");
			} else if (text.find("World vibration:") != std::string::npos) {
				selection.setText("World vibration: " + std::to_string((int)(renderer3D->getCurrentWorldVibrationEffect() * 100)) + "%");
			} else if (text.find("Color shift:") != std::string::npos) {
				selection.setText("Color shift: " + std::to_string((int)(renderer3D->getCurrentColorShiftEffect() * 100)) + "%");
			} else if (text.find("Kaleidoscope:") != std::string::npos) {
				selection.setText("Kaleidoscope: " + std::to_string((float)(renderer3D->getCurrentKaleidoscopeEffect() * 100)) + "%");
			} else if (text.find("Noise effect:") != std::string::npos) {
				selection.setText("Noise effect: " + std::to_string((int)(renderer3D->getCurrentNoiseEffect() * 100)) + "%");
			} else if (text.find("Greyscale effect:") != std::string::npos) {
				selection.setText("Greyscale effect: " + std::to_string((int)(renderer3D->getCurrentGreyscaleEffect() * 100)) + "%");
			} else if (text.find("Stretch effect:") != std::string::npos) {
				selection.setText("Stretch effect: " + std::to_string((int)(renderer3D->getCurrentStretchEffect() * 100)) + "%");
			} else if (text.find("Bitcrush effect:") != std::string::npos) {
				selection.setText("Bitcrush effect: " + std::to_string((int)(renderer3D->getCurrentBitcrushEffect() * 100)) + "%");
			} else if (text.find("World scrambler:") != std::string::npos) {
				selection.setText("World scrambler: " + std::to_string((int)(renderer3D->getCurrentWorldScramblerEffect() * 100)) + "%");
			} else if (text.find("Fractal drug:") != std::string::npos) {
				selection.setText("Fractal drug: " + std::to_string((int)(renderer3D->getCurrentFractalEffect() * 100)) + "%");
			} else if (text.find("Depth perception:") != std::string::npos) {
				selection.setText("Depth perception: " + std::to_string((int)(renderer3D->getCurrentDepthPerceptionEffect() * 100)) + "%");
			} else if (text.find("Dissolve effect:") != std::string::npos) {
				selection.setText("Dissolve effect: " + std::to_string((int)(renderer3D->getCurrentDissolveEffect() * 100)) + "%");
			}
		}
	}
	
	return true;
}

}
