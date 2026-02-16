#include "Text/TextInput.h"
#include "Game/Util.h"
#include "Game/Game.h"
#include "Game/Controller.h"
#include "Game/Sfx.h"
#include "Game/Mouse.h"
#include "Game/Fonts.h"
#include "Game/Easing.h"

namespace nyaa {

TextInput::TextInput(float posX, float posY, std::string nname, std::string defaultInput)
	:name(nname, *Fonts::MainFont, 26)
	,characters(" ABCDEFGHI\nJKLMNOPQRS\nTUVWXYZ_!?", *Fonts::MonoFont, 80)
	,input(defaultInput, *Fonts::MainFont, 50)
	,selIndex(0)
	,typing(true)
	,selRectOpacityUp(false)
	,selRectOpacityBreatheMs(500)
{
	name.setPosition(Game::ScreenWidth / 2.0f, posY);
	setNameOrigin();
	name.setOutlineThickness(5);
	name.setOutlineColor(sf::Color::Black);
	name.setFillColor(sf::Color::Red);
	input.setOutlineThickness(5);
	input.setOutlineColor(sf::Color::Black);
	input.setFillColor(sf::Color::Cyan);
	input.setPosition(Game::ScreenWidth / 2.0f, name.getPosition().y + 35);
	characters.setPosition(Game::ScreenWidth / 2.0f, input.getPosition().y + 35);
	characters.setOrigin(characters.getLocalBounds().width / 2, 0);
	characters.setFillColor(sf::Color::White);
	characters.setOutlineColor(sf::Color::Black);
	characters.setOutlineThickness(5);
	auto ab = Fonts::MonoFont->getGlyph(L'1', characters.getCharacterSize() * 1.4, false/*0, 0*/).textureRect;
	selRect.setFillColor(sf::Color(255, 255, 255, 150));
	selRect.setSize(sf::Vector2f(ab.width * 1.1, ab.height));
}

void TextInput::update()
{
	guip = this;

	if (guip_eof == this)
	{
		sf::Vector2i cmousepos = sf::Vector2i{ Mouse::Pos_X, Mouse::Pos_Y };
		if (typing)
		{
			bool up = Kb::IsKeyFirstFrame(KB::Up) || (Controller::dpy == 100 && Controller::dpyFrames == 1);
			bool down = Kb::IsKeyFirstFrame(KB::Down) || (Controller::dpy == -100 && Controller::dpyFrames == 1);
			bool wallnothit = true;
			if ((up || down) && !(up && down))
			{
				sf::Vector2f ccp = characters.findCharacterPos(selIndex);
				int closest = selIndex;
				float closest_dist = std::numeric_limits<float>::infinity();
				float srs = (selRect.getSize().y);
				for (size_t i = 0; i < characters.getString().getSize(); ++i)
				{
					char ch = characters.getString().getData()[i];
					if (ch == '\n' || (ch == ' ' && i > 0)) continue;
					sf::Vector2f fcp = characters.findCharacterPos(i);
					float dist = Util::Dist(ccp.x, ccp.y + (up ? -srs : srs), fcp.x, fcp.y);
					if (dist < closest_dist)
					{
						closest = i;
						closest_dist = dist;
					}
				}

				if (selIndex == closest) wallnothit = false;

				selIndex = closest;
				Sfx::CursorMove->play();
			}
			else if (Kb::IsKeyFirstFrame(KB::Left) || (Controller::dpx == -100 && Controller::dpxFrames == 1))
			{
				if (selIndex >= 1 && characters.getString().getData()[selIndex - 1] == '\n') selIndex -= 2;
				else --selIndex;
				Sfx::CursorMove->play();
			}
			else if (Kb::IsKeyFirstFrame(KB::Right) || (Controller::dpx == 100 && Controller::dpxFrames == 1))
			{
				++selIndex;
				Sfx::CursorMove->play();
			}
			else if (Kb::IsKeyFirstFrame(KB::Return) || Mouse::LeftFrames == 1 || Controller::BtnFrames[Btn_X] == 1)
			{
				auto rc = characters.getString().getData()[selIndex];
				char sc = Kb::IsKeyDown(KB::LShift) ? rc : tolower(rc);
				input.setString(input.getString() + sc);
				//Sfx::CursorSel->play();
				Sfx::Pick->play();
				selRectOpacityClock.restart();
				selRectOpacityUp = true;
			}
			else if ((Kb::IsKeyFirstFrame(KB::BackSpace) || Controller::BtnFrames[Btn_Circle] == 1) && input.getString().getSize() > 0)
			{
				backspc();
			}

			if (down && !wallnothit) typing = false;

			if (Mouse::Moved)
			{
				int befindx = selIndex;
				int closest = selIndex;
				float closest_dist = std::numeric_limits<float>::infinity();
				auto srs = (selRect.getSize());
				for (size_t i = 0; i < characters.getString().getSize(); ++i)
				{
					char ch = characters.getString().getData()[i];
					if (ch == '\n' || (ch == ' ' && i > 0)) continue;
					sf::Vector2f fcp = characters.findCharacterPos(i);
					float dist = Util::Dist(cmousepos.x - (srs.x / 2), cmousepos.y - (srs.y), fcp.x, fcp.y);
					if (dist < closest_dist)
					{
						closest = i;
						closest_dist = dist;
					}
				}

				if (selIndex == closest) wallnothit = false;

				selIndex = closest;
				if (selIndex != befindx) Sfx::CursorMove->play();
			}
		}
	}
	if (selIndex < 0) selIndex = characters.getString().getSize() - 1;
	if (characters.getString().getData()[selIndex] == '\n') selIndex++;
	if (selIndex >= (int)characters.getString().getSize()) selIndex = 0;
	selIndex = Util::Clamp(selIndex, (int)0, (int)characters.getString().getSize() - 1);
	auto cpos = characters.findCharacterPos(selIndex);
	cpos.y += selRect.getSize().y * 0.4;
	selRect.setPosition(cpos);
	if (selRectOpacityClock.getElapsedMilliseconds() >= selRectOpacityBreatheMs)
	{
		selRectOpacityClock.restart();
		selRectOpacityUp = !selRectOpacityUp;
	}
	sf::Color nc = selRect.getFillColor();
	nc.a = Util::Scale(selRectOpacityClock.getElapsedMilliseconds(), 0, selRectOpacityBreatheMs, selRectOpacityUp ? 100 : 255, selRectOpacityUp ? 255 : 100);
	selRect.setFillColor(nc);
}

void TextInput::render(RendTarget* renderTarget)
{
	renderTarget->draw(name);
	input.setOrigin(input.getLocalBounds().width / 2, 0);
	renderTarget->draw(input);
	renderTarget->draw(characters);
	if (typing) {
		renderTarget->draw(selRect);
		// Draw overlay of selected character above the highlight
		if (selIndex >= 0 && selIndex < (int)characters.getString().getSize()) {
			char selectedChar = characters.getString().getData()[selIndex];
			if (selectedChar != '\n' && selectedChar != ' ') {
				sf::Text overlayText(std::string(1, selectedChar), *Fonts::MonoFont, 100);
				overlayText.setStyle(sf::Text::Bold);
				auto fc = sf::Color::Yellow;
				fc.a = selRect.getFillColor().a;
				overlayText.setFillColor(fc);
				overlayText.setOutlineColor(sf::Color(fc.a, 20 + fc.a / 2, 10, 255));
				overlayText.setOutlineThickness(10);
				// Position above the highlight rectangle
				auto origin = sf::Vector2f(selRect.getSize().x / 2, selRect.getSize().y / 2);
				sf::Vector2f selPos = selRect.getPosition();
				overlayText.setOrigin(origin.x, origin.y * 2);
				auto sc = Easing::ApplyClamped(Ease_Out, 0.5f + fc.a / 255.f / 2, 0, 1);
				overlayText.setScale(sc, sc);
				overlayText.setPosition(selPos.x + (origin.x) - 5, selPos.y + origin.y - 5);
				renderTarget->draw(overlayText);
			}
		}
	}
}

void TextInput::reset()
{
	input.setString("");
	typing = true;
	selIndex = 0;
}

void TextInput::backspc()
{
	input.setString(input.getString().substring(0, input.getString().getSize() - 1));
	Sfx::Close->play();
}

void TextInput::setNameOrigin()
{
	name.setOrigin(name.getLocalBounds().width / 2, 0);
}

void TextInput::setName(std::string name)
{
	this->name.setString(name);
}

void TextInput::getCharactersLocalBounds(float* outLeft, float* outTop, float* outWidth, float* outHeight)
{
	auto lb = characters.getLocalBounds();
	if(outLeft) *outLeft = lb.left;
	if(outTop) *outTop = lb.top;
	if(outWidth) *outWidth = lb.width;
	if(outHeight) *outHeight = lb.height;
}

void TextInput::getCharactersGlobalBounds(float* outLeft, float* outTop, float* outWidth, float* outHeight)
{
	auto lb = characters.getGlobalBounds();
	if(outLeft) *outLeft = lb.left;
	if(outTop) *outTop = lb.top;
	if(outWidth) *outWidth = lb.width;
	if(outHeight) *outHeight = lb.height;
}

}
