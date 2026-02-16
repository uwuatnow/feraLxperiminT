#include "Screen/LoadGameScreen.h"
#include "Screen/NewGameScreen.h"
//#include "Screen/InGameScreen.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <Game/Game.h>
#include <Game/Util.h>
#include <Game/SelectionPrompt.h>
#include <fstream>
#include <sstream>
#include "Game/Fonts.h"

namespace nyaa {

LoadGameScreen* LoadGameScreen::Instance = nullptr;

LoadGameScreen::LoadGameScreen()
    :overwriteWithNewFileModeOn(false)
    ,selPrompt(new SelectionPrompt({200, Game::ScreenHeight * 0.35f, 100, Game::ScreenHeight * 0.5f}))
{
    Instance = this;

    selPrompt->add("<- back", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
    {
        if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
        Screen::LastScreen->switchTo();
        return SelPResp_DidSomething;
    });

    for (int i = 0; i < 6; i++) {
        selPrompt->add(Util::Format("Slot %d", i + 1), [&](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
        {
            if (d != Direction_Center)
            {
                return SelPResp_Neutral;
            }
            G->selectedSaveSlot = s.index - 1;
            if (!overwriteWithNewFileModeOn)
            {
                NewGameScreen::Instance->skipAndLoadSave = true;
            }
            NewGameScreen::Instance->switchTo();
            return SelPResp_DidSomething;
        });
    }
}

LoadGameScreen::~LoadGameScreen()
{
    delete selPrompt;
}

void LoadGameScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
    // Draw gradient background
    sf::RectangleShape bg(sf::Vector2f((float)Game::ScreenWidth, (float)Game::ScreenHeight));
    bg.setFillColor(sf::Color(25, 35, 45));
    renderTarget->draw(bg);
    
    // Draw decorative elements
    for(int i = 0; i < 8; i++) {
        sf::CircleShape circle(20 + i * 5);
        circle.setFillColor(sf::Color(40, 60, 80, 30 - i * 3));
        circle.setPosition(Game::ScreenWidth * 0.8f + i * 15, Game::ScreenHeight * 0.2f + i * 20);
        renderTarget->draw(circle);
    }
    
    // Draw title
    sf::Text title(overwriteWithNewFileModeOn ? "new game" : "load game", *Fonts::MainFont, 48);
    title.setFillColor(sf::Color::White);
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(3);
    auto titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    title.setPosition(Game::ScreenWidth / 2, Game::ScreenHeight * 0.15f);
    renderTarget->draw(title);
    
    selPrompt->update();
    selPrompt->render(renderTarget);
	if(
		(
			Kb::IsKeyReleased(KB::Escape)
			|| Kb::IsKeyReleased(KB::Q)
		)
		&& myLastScreen
	)
	{
		myLastScreen->switchTo();
	}
}

bool LoadGameScreen::onEnter()
{
    selPrompt->selectedIndex = 1;
    return true;
}

bool LoadGameScreen::onLeave()
{
    overwriteWithNewFileModeOn = false;
    return true;
}

}