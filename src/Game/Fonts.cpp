#include "Fonts.h"
#include "Game/Game.h"
#include "Game/Util.h"

namespace nyaa {

sf::Font* Fonts::MainFont = nullptr;
sf::Font* Fonts::MonoFont = nullptr;
sf::Font* Fonts::OSDFont = nullptr;

void Fonts::Init()
{
	MainFont = new sf::Font();
	MonoFont = new sf::Font();
	OSDFont = new sf::Font();
	
	MainFont->loadFromFile(Util::Format("%s" "font.ttf", Game::ResDir));
	MonoFont->loadFromFile(Util::Format("%s" "FreeMono.otf", Game::ResDir));
	OSDFont->loadFromFile(Util::Format("%s" "osd_mono.otf", Game::ResDir));
}

void Fonts::Cleanup()
{
	delete MainFont;
	delete MonoFont;
	delete OSDFont;
	MainFont = nullptr;
	MonoFont = nullptr;
	OSDFont = nullptr;
}

}