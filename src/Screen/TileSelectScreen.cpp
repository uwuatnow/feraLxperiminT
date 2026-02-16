#include "Screen/TileSelectScreen.h"
#include "Screen/InGameScreen.h"
#include "Game/Util.h"
#include "Game/Game.h"
#include "Game/Controller.h"
#include "Game/Sfx.h"
#include "Mission/Mission.h"
#include "Game/Mouse.h"
#include "Game/Fonts.h"
#include "Texture/Sheet.h"

#if DEBUG

namespace nyaa {

TileSelectScreen* tileSelectScreen = nullptr;

TileSelectScreen::TileSelectScreen()
{
}

TileSelectScreen::~TileSelectScreen()
{
}

void TileSelectScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	int tx = 0, ty = 0;
	
	auto mp = sf::Vector2i{ Mouse::Pos_X, Mouse::Pos_Y };
	tx = mp.x / Map::TileSizePixels;
	ty = mp.y / Map::TileSizePixels;
	
	if (Mouse::LeftFrames == 1)
	{
		IGS->tileSelectTileId = (ty * IGS->sheet->sizeX) + tx;
	}
	
	sf::Vector2f txp((float)(tx * Map::TileSizePixels),(float)(ty * Map::TileSizePixels));
	sf::RectangleShape tsh;
	tsh.setFillColor(sf::Color(255, 255, 255, 100));
	tsh.setOutlineColor(sf::Color(0, 0, 0, 255));
	tsh.setOutlineThickness(2);
	tsh.setPosition(txp);
	tsh.setSize(sf::Vector2f(Map::TileSizePixels ,Map::TileSizePixels));
	int tid = (ty * IGS->sheet->sizeX) + tx;
	
	// Handle keyboard layer changes
	if (Kb::IsKeyReleased(KB::Dash) || Controller::BtnFrames[Btn_LBtn] == 1)
	{
		IGS->selectedLayer--;
	}
	else if (Kb::IsKeyReleased(KB::Equal) || Controller::BtnFrames[Btn_RBtn] == 1)
	{
		IGS->selectedLayer++;
	}
	IGS->selectedLayer = Util::Clamp(IGS->selectedLayer, 0, 2);
	
	// Handle D-pad and keyboard tile selection
	auto dpxA = Controller::dpxFrames == 1 || (Controller::dpxFrames > 9 && (Controller::dpxFrames % 5 == 0));
	auto dpyA = Controller::dpyFrames == 1 || (Controller::dpyFrames > 9 && (Controller::dpyFrames % 5 == 0));
	
	// Check for keyboard arrow keys
	bool keyLeft = Kb::IsKeyFirstFrame(KB::Left);
	bool keyRight = Kb::IsKeyFirstFrame(KB::Right);
	bool keyUp = Kb::IsKeyFirstFrame(KB::Up);
	bool keyDown = Kb::IsKeyFirstFrame(KB::Down);
	
	if (dpxA || dpyA || keyLeft || keyRight || keyUp || keyDown)
	{
		if (Controller::dpx > 0 || keyRight) // D-pad right or Right arrow key
		{
			// Move right one column in the tile sheet
			int currentX = IGS->tileSelectTileId % IGS->sheet->sizeX;
			int currentY = IGS->tileSelectTileId / IGS->sheet->sizeX;
			currentX = (currentX < ((int)IGS->sheet->sizeX - 1)) ? currentX + 1 : 0; // Wrap around
			IGS->tileSelectTileId = (currentY * IGS->sheet->sizeX) + currentX;
			Controller::dpy = 0;
		}
		else if ((Controller::dpy > 0 && !Controller::dpx) || keyUp) // D-pad up or Up arrow key
		{
			// Move up one row in the tile sheet
			int currentX = IGS->tileSelectTileId % IGS->sheet->sizeX;
			int currentY = IGS->tileSelectTileId / IGS->sheet->sizeX;
			currentY = (currentY > 0) ? currentY - 1 : IGS->sheet->sizeY - 1; // Wrap around
			IGS->tileSelectTileId = (currentY * IGS->sheet->sizeX) + currentX;
		}
		else if (Controller::dpy < 0 || keyDown) // D-pad down or Down arrow key
		{
			// Move down one row in the tile sheet
			int currentX = IGS->tileSelectTileId % IGS->sheet->sizeX;
			int currentY = IGS->tileSelectTileId / IGS->sheet->sizeX;
			currentY = (currentY < ((int)IGS->sheet->sizeY - 1)) ? currentY + 1 : 0;
			IGS->tileSelectTileId = (currentY * IGS->sheet->sizeX) + currentX;
		}
		else if (Controller::dpx < 0 || keyLeft) // D-pad left or Left arrow key
		{
			// Move left one column in the tile sheet
			int currentX = IGS->tileSelectTileId % IGS->sheet->sizeX;
			int currentY = IGS->tileSelectTileId / IGS->sheet->sizeX;
			currentX = (currentX > 0) ? currentX - 1 : IGS->sheet->sizeX - 1; // Wrap around
			IGS->tileSelectTileId = (currentY * IGS->sheet->sizeX) + currentX;
		}
	}
	sf::Sprite sheetSp{IGS->sheet->tex};
	renderTarget->draw(sheetSp);
	sf::Text dt(Util::Format("hov:%d\ncur:%d\nlayer:%d", tid, IGS->tileSelectTileId, IGS->selectedLayer), *Fonts::OSDFont, 30);
	dt.setFillColor(sf::Color(255, 255, 0, 150));
	dt.setOutlineColor(sf::Color::Black);
	dt.setOutlineThickness(3);
	auto lb = dt.getLocalBounds();
	dt.setPosition(10,Game::ScreenHeight - (lb.height * 1.25f));
	renderTarget->draw(tsh);
	tsh.setFillColor(sf::Color(255, 0, 0, 160));
	auto tshPos = sf::Vector2f{};
	IGS->sheet->getTexCoordsFromId(IGS->tileSelectTileId, &tshPos.x, &tshPos.y);
	tsh.setPosition(tshPos);
	renderTarget->draw(tsh);
	renderTarget->draw(dt);
	if (Kb::IsKeyReleased(KB::Escape)
		|| Controller::BtnFrames[Btn_Triangle] == 1
	) 
	{
		Sfx::Close->play();
		IGS->switchTo();
	}
}

bool TileSelectScreen::onEnter()
{
	return true;
}

}
#endif
