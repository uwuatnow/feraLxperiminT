#include "Game/HealthBar.h"
#include "Game/Util.h"
#include "Game/Game.h"
#include "Game/Fonts.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace nyaa {

HealthBar::HealthBar(std::string label, unsigned char colR, unsigned char colG, unsigned char colB)
	:colR(colR)
	,colG(colG)
	,colB(colB)
	,label(label)
	,realVal(nullptr)
	,renderVal(0.0f)
	,alphaUp(false)
	,alphaMs(650.0f)
	,hbar(TextureMan::tman->get("hbar"))
	,hbar_end(TextureMan::tman->get("hbar_end"))
	,alpha(100)
{
}

void HealthBar::update()
{
	if (G->curScreenFramesPassed == 0)
	{
		updateTimer.reset();
		alphaTimer.reset();
	}

	updateTimer.update();
	auto msp = updateTimer.millis() / 10;

	if (fabsf(renderVal - *realVal) > 2 * G->frameDeltaMillis / 14)
	{
		if (renderVal < *realVal) renderVal += msp;
		else renderVal -= msp;
	}

	renderVal = Util::Clamp(renderVal, 8, 100);

	updateTimer.zero();

	alphaTimer.update();

	float bottom = 150, top = 200;
	auto acm = alphaTimer.millis();
	float a = Util::Scale(acm, 0, alphaMs, alphaUp ? bottom : top, alphaUp ? top : bottom);

	if (acm >= alphaMs)
	{
		alphaTimer.zero();
		alphaUp = !alphaUp;
		a = alphaUp ? bottom : top;
	}
	alpha = a;
}

void HealthBar::render(float posX, float posY, RendTarget* renderTarget)
{
	sf::Text labelText;
	labelText.setString(label);
	labelText.setFont(*Fonts::MonoFont);
	labelText.setCharacterSize(7);
	//std::printf("%s bar: %f\n", labelText.getString().toAnsiString().c_str(), *realVal);
	
	labelText.setOutlineThickness(1.0f);
	labelText.setOutlineColor(sf::Color(0, 0, 0, 100));
	labelText.setFillColor(sf::Color(255, 255, 255, 100));
	labelText.setOrigin(sf::Vector2f(2, 0));
	labelText.setPosition(posX, posY);

	sf::Sprite hbar_sp;
	sf::Sprite hbar_end_sp;
	hbar_sp.setTexture(hbar.tex, true);
	hbar_end_sp.setTexture(hbar_end.tex, true);

	sf::Color nc = {colR, colG, colB};
	nc.a = alpha;
	hbar_sp.setColor(nc);
	hbar_end_sp.setColor(nc);

	auto hbesfr = hbar_end_sp.getLocalBounds();
	//auto hbsfr = hbar_sp.getLocalBounds();
	hbar_sp.setOrigin(sf::Vector2f(hbesfr.width / 2, 0));
	hbar_end_sp.setOrigin(sf::Vector2f(-(hbesfr.width / 2), 0));

	hbar_sp.setPosition(posX, posY);
	auto hbep = sf::Vector2f(
		Util::Scale(renderVal, 0, 100, 0, hbar.tex.getSize().x),
		posY
	);
	hbar_sp.setTextureRect(sf::IntRect(0, 0, hbep.x, hbar.tex.getSize().y));
	hbar_end_sp.setPosition(sf::Vector2f{(float)((int)hbep.x), hbep.y});
	renderTarget->draw(hbar_sp);
	renderTarget->draw(hbar_end_sp);
	renderTarget->draw(labelText);
}

}