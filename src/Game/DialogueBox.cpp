#include "DialogueBox.h"
#include "Game/Game.h"
#include "Game/Util.h"
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include <cassert>
#include "Game/Controller.h"
#include "Game/Sfx.h"
#include "Game/Mouse.h"
#include "Game/Fonts.h"

namespace nyaa {

DialogueBox::Msg::Msg(DialogueBox* host, Actor* from, std::string msg, DBPostMsgCback pmCb)
	:host(host)
	,actor(from)
	,pmCb(pmCb)
	,scrollLnY(0)
	,lines(1)
	,scrollTime(200)
	,scrollable(false)
{
	sf::Font& tfont = *Fonts::MonoFont;
	sf::Text tt(msg, tfont);
	float widthLim = host->bg.getSize().x * 0.9;
	lines = 1;
	this->msg.setFont(tfont);
	this->msg.setOutlineThickness(1);
	this->msg.setFillColor(sf::Color::Transparent);
	this->msg.setOutlineColor(sf::Color::White);
	if (tt.getLocalBounds().width >= widthLim)
	{
		size_t lendp = 0, lspcti = 0;
		auto mlen = msg.length();
		for (size_t ti = 0; ti < mlen; ++ti)
		{
			char cc = msg.at(ti);
			std::string sub(msg.substr(lendp, ti - lendp));
			if (cc == ' ')
			{
				lspcti = ti;
				continue;
			}
			sf::Text tempText(sub, tfont);
			float w = tempText.getLocalBounds().width;
			if (w >= widthLim)
			{
				((char*)msg.data())[lspcti] = '\n';
				lendp = lspcti;
				++lines;
			}
		}
	}
	this->msg.setString(msg);
	if (lines > 3)
	{
		scrollable = true;
	}

	scrollTimer.restartThresholdMs = scrollTime;
}

//DialogueBox::Msg::Msg(DialogueBox* host, Actor* from, std::string msg, sol::function pmCbLUA)
//	:pmCbLUA(pmCbLUA)
//{
//	Msg(host, from, msg);
//}

void DialogueBox::Msg::update()
{
	scrollTimer.update();
}

void DialogueBox::Msg::render(float posX, float posY, char opacity)
{
	auto MainFont = msg.getFont();
	assert(MainFont);
	if (!MainFont)
	{
		return;
	}
	float lh = MainFont->getLineSpacing(msg.getCharacterSize());
	int scr = scrollLnY - 1;
	float val = Util::Scale(Util::Clamp(Easing::Apply(Ease_In, scrollTimer.millis(), 0, scrollTime), 0, scrollTime),
	    0, scrollTime, lh * scr, lh * (scr + 1));
	if (val < 0)
	{
		val = 0;
	}
	posY -= val;
	msg.setPosition(posX, posY);
	//msg.setOutlineColor(sf::Color(255, 255, 255, opacity));
	host->msgsTex.draw(msg);
}

bool DialogueBox::Msg::isDone()
{
	return scrollLnY >= lines - 3;
}

bool DialogueBox::Msg::scroll()
{
	if (!scrollable || (isDone() && scrollTimer.millis() >= scrollTime)) return true;
	if (scrollTimer.millis() >= scrollTime/* || scrollLnY == 0*/)
	{
		++scrollLnY;
		Sfx::Pick->play();
		scrollTimer.zero();
	}
	return false;
}

DialogueBox::DialogueBox()
	:endText("", *Fonts::MainFont, 12)
	,closeCallback(nullptr)
	,curMsg(0)
	,showing(false)
	,slideTime(350)
	,fadeMs(300)
{
	endText.setFillColor(sf::Color(255, 255, 255, 150));
	endText.setOutlineColor(sf::Color(0, 0, 0, 150));
	endText.setOutlineThickness(2);
	bg.setOutlineThickness(4);
	bg.setOutlineColor(sf::Color::Magenta);
	bg.setFillColor(sf::Color(10, 10, 10, 170));
	resize();
}

DialogueBox::~DialogueBox()
{
	
}

void DialogueBox::resize()
{
	auto sz = sf::Vector2f(Game::ScreenWidth - 30, 100);
	bg.setSize(sz);
	auto bgb = bg.getLocalBounds();
	bg.setOrigin(bgb.width / 2, bgb.height / 2);
	auto ot2 = bg.getOutlineThickness() / 2;
	msgsTex.create(sz.x - ot2, sz.y - ot2);
	msgsTex.setSmooth(true);
	texSp.setTexture(msgsTex.getTexture());
	auto mtb = texSp.getLocalBounds();
	texSp.setOrigin(sf::Vector2f(mtb.width / 2, mtb.height / 2));
}

void DialogueBox::updateEndText()
{
	auto msg = msgs.begin();
	if (msgs.size() < 1) return;
	std::advance(msg, curMsg);
	if (msg->scrollable && !msg->isDone()) endText.setString("Click to scroll down.");
	else if (curMsg == msgs.size() - 1) endText.setString("Click to close.");
	else endText.setString("Click go to the next message.");
}

void DialogueBox::clear()
{
	msgs.clear();
	curMsg = 0;
	showing = false;
}

void DialogueBox::add(Actor* from, std::string msg, DBPostMsgCback pmCb)
{
	msgs.push_back(Msg(this, from, msg, pmCb));
}

//void DialogueBox::add(Actor* from, std::string msg, sol::function pmCbLUA)
//{
//	auto& m = msgs.emplace_back(this, from, msg);
//	m.pmCbLUA = pmCbLUA;
//}

void DialogueBox::show()
{
	curMsg = 0;
	if (msgs.size() > 0) showing = true;
	updateEndText();
}

void DialogueBox::update()
{
	boxFadeTimer.update2(!showing);
	slideTimer.update();
	if (showing)
	{
		guip = this;
	}
	if (showing && guip_eof == this) {
		if (Mouse::LeftFrames == 1 || Kb::IsKeyFirstFrame(KB::Space) || Controller::BtnFrames[Btn_X] == 1)
		{
			next();
		}
#if DEBUG
		if (Kb::IsKeyFirstFrame(KB::S) || Controller::BtnFrames[Btn_Triangle] == 1) /*skip text box*/
		{
			std::printf("Skipped text box\n");
			Sfx::Close->play();
			clear();
		}
#endif
	}

	if (msgs.size() < 1)
	{
		showing = false;
		curMsg = 0;
	}
}

void DialogueBox::render(RendTarget* renderTarget)
{
	if (msgs.size() < 1)
	{
		return;
	}
	if (boxFadeTimer.millis() == 0 && !showing)
	{
		return;
	}

	msgsTex.clear(sf::Color::Transparent);

	auto fr = bg.getLocalBounds();

	auto cfms = Util::Clamp(boxFadeTimer.millis(), 0, fadeMs);
	float a = Util::Scale(cfms, 0, fadeMs, 0, 150);
	float a2 = Util::Scale(cfms, 0, fadeMs, 0, 255);
	float sc = Util::Scale(Easing::Apply(/*showing ? EasingType::OutBounce : */Ease_In, cfms, 0, fadeMs), 0, fadeMs, 0, 1);

	//draw box
	auto pos = sf::Vector2f(Game::ScreenWidth / 2, Game::ScreenHeight - (fr.height / 2) - 15);
	bg.setPosition(pos);
	auto bgCol = bg.getFillColor();
	bgCol.a = a;
	bg.setFillColor(bgCol);
	auto bgOlCol = bg.getOutlineColor();
	bgOlCol.a = a;
	bg.setOutlineColor(bgOlCol);
	bg.setScale(sc, sc);
	renderTarget->draw(bg);

	float slide = Easing::ApplyClamped(Ease_In, slideTimer.millis(), 0, slideTime);

	//draw msg
	curMsg = Util::Clamp(curMsg, (unsigned int)0, (unsigned int)msgs.size() - 1);
	auto msg = msgs.begin();
	std::advance(msg, curMsg);
	DialogueBox::Msg* lmsg = nullptr;
	if (curMsg > 0) {
		auto lm = msgs.begin();
		std::advance(lm, curMsg - 1);
		lmsg = &*lm;
	}
	if (lmsg) {
		lmsg->render(5 - Util::Scale(slide, 0, slideTime, 0, lmsg->msg.getLocalBounds().width), -5), Util::Scale(slide, 0, slideTime, 255, 0);
	}
	msg->update();
	if (lmsg) {
		msg->render(lmsg->msg.getPosition().x + lmsg->msg.getLocalBounds().width + 5, -5);
	}
	else msg->render(5, -5);

	msgsTex.display();

	auto bgot = bg.getOutlineThickness();
	texSp.setPosition(sf::Vector2f(pos.x - bgot, pos.y - bgot));
	texSp.setColor(sf::Color(255, 255, 255, a2));
	texSp.setScale(sf::Vector2f(sc, sc));
	renderTarget->draw(texSp);

	endText.setPosition(sf::Vector2f(pos.x - (fr.width / 2) + 2, pos.y + (fr.height / 2)/* - endText.getLocalBounds().height - 5*/));
	endText.setFillColor(sf::Color(255, 255, 255, a2));
	endText.setOutlineColor(sf::Color(0, 0, 0, a2));
	renderTarget->draw(endText);
}

void DialogueBox::next()
{
	auto msg = msgs.begin();
	std::advance(msg, curMsg);
	if (curMsg > 0 && slideTimer.millis() < slideTime)
	{
		return;
	}
	if (msg->scroll())
	{
		if (msg->pmCb)msg->pmCb(*msg);
		/*else if (msg->pmCbLUA.valid())msg->pmCbLUA();*/
		if (curMsg < msgs.size() - 1)
		{
			slideTimer.zero();
		}
		if (curMsg + 1 > msgs.size() - 1)
		{
			if (boxFadeTimer.millis() > fadeMs)
			{
				if (closeCallback)
				{
					closeCallback();
					closeCallback = nullptr;
				}
				/*else if (LUAcloseCallback.valid()) {
					LUAcloseCallback();
					LUAcloseCallback.reset();
				}*/
				showing = false;
				IGS->player->isControllable = true;
				boxFadeTimer.setMillis(fadeMs);
			}
		}
		else ++curMsg;
		Sfx::Pick->play();
	}
	updateEndText();
}

}
