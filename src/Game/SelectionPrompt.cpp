#include "Game/SelectionPrompt.h"
#include "Game/Util.h"
#include "Game/Controller.h"
#include "Game/Kb.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include "Game/Sfx.h"
#include "Game/Game.h"
#include "Game/Mouse.h"
#include "Game/Easing.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>
#include "Game/Fonts.h"

namespace nyaa {

SelectionPrompt::SelectionPrompt(float rectLeft, float rectTop, float rectWidth, float rectHeight, bool vertical)
	:rectLeft(rectLeft)
	,rectTop(rectTop)
	,rectWidth(rectWidth)
	,rectHeight(rectHeight)
	,selectedIndex(0)
	,vertical(vertical)
	,upFail(false)
	,scrollOffset(0.0f)
	,scrollStartY(0.0f)
	,scrollTargetY(0.0f)
	,showBreadcrumb(false)
{

}

SelectionPrompt::~SelectionPrompt() 
{

}

SelectionPrompt::Selection& SelectionPrompt::add(std::string text, SPCallback cb, bool enabled) 
{
	Selection* prev = selections.size() > 0 ? &selections.back() : nullptr;
	selections.emplace_back(*this, text, enabled);
	Selection& s = selections.back();
	s.callback = cb;
	s.index = selections.size() - 1;
	s.update(false);
	s.bef = prev;
	if (prev) prev->next = &s;
	return s;
}

//int SelectionPrompt::getClicked()
//{
//	int i = 0;
//	int ret = -1;
//	for(auto s : selectionVec)
//	{
//		if(s->isClicked)
//		{
//			s->isClicked = false;
//			ret = i;
//		}
//		if(ret != -1) s->isClicked = false;
//		++i;
//	}
//
//	return ret;
//}

void SelectionPrompt::update(bool focus)
{
	upFail = false;

	// Timer-based smooth scrolling
	float t = scrollTimer.getElapsedMilliseconds() / 200.0f;
	if (t < 1.0f) {
		t = t * t * (3.0f - 2.0f * t); // smoothstep
		scrollOffset = scrollStartY + (scrollTargetY - scrollStartY) * t;
	} else {
		scrollOffset = scrollTargetY;
	}

	if (focus) guip = this;

	size_t size = selections.size();

	if (guip_eof == this && size > 0) {
		selectedIndex = Util::Clamp(selectedIndex, (unsigned int)0, (unsigned int)size - 1);
		auto selp = selections.begin();
		std::advance(selp, selectedIndex);
		/*auto dpx = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX);
		auto dpy = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY);
		std::cout<<std::format("{} {}\n",dpx,dpy);*/
		//assert(a == 0);
		bool up = Kb::IsKeyFirstFrame(KB::Up)
			|| (Controller::dpy == 100 && Controller::dpyFrames == 1)
			|| Kb::IsKeyFirstFrame(KB::W);
		bool down = Kb::IsKeyFirstFrame(KB::Down)
			|| (Controller::dpy == -100 && Controller::dpyFrames == 1)
			|| Kb::IsKeyFirstFrame(KB::S);
		
		// Mouse wheel scrolling
		if (Mouse::WheelDelta != 0) {
			if (Mouse::WheelDelta > 0) up = true;
			else down = true;
		}

		bool left = Kb::IsKeyFirstFrame(KB::Left)
			|| Kb::IsKeyFirstFrame(KB::A);
		bool right = Kb::IsKeyFirstFrame(KB::Right)
			|| Kb::IsKeyFirstFrame(KB::D);
		if (size > 1)
		{
			if (up)
			{
				/*if(selectedIndex == 0) selectedIndex = size - 1;
				else --selectedIndex;*/
				if (selectedIndex > 0)
				{
					--selectedIndex;
					updateScrollOffset();
					Sfx::CursorMove->play();
				}
				else
				{
					up = false;
					upFail = true;
				}
			}
			else if (down)
			{
				/*if(selectedIndex >= size - 1) selectedIndex = 0;
				else ++selectedIndex;*/
				if (selectedIndex < size - 1)
				{
					++selectedIndex;
					updateScrollOffset();
					Sfx::CursorMove->play();
				}
				else {
					down = false;
				}
			}
		}

		sf::Vector2i mpos = sf::Vector2i{ Mouse::Pos_X, Mouse::Pos_Y };
		unsigned int i = 0;
		for (auto it = selections.begin(); it != selections.end(); )
		{
			auto itNext = std::next(it);
			bool bleft = left;
			bool bright = right;
			auto&s=*it;
			bool seld = selectedIndex == i;
			//if(seld) guip = &s;
			bool hovd = G->winFocused && s.text.getGlobalBounds().contains((sf::Vector2f)mpos);
			auto og = s.text.getFillColor().a >= 225;
			bool msel = !seld && hovd && Mouse::Moved;
			if (up || down || msel)
			{
				//s.animClock.restart();
				//s.animTimer.zero();
				//s.animUp = true;
			}
			bool mouseScrolled = false;
			switch(s.mouseScroll)
			{
			case Direction_Left:
			{
				left = true;
				mouseScrolled = true;
				break;
			}
			case Direction_Right:
			{
				right = true;
				mouseScrolled = true;
				break;
			}
			default:
			{
				break;
			}
			}
			s.mouseScroll = Direction_None;
			if (msel && selectedIndex != i)
			{
				selectedIndex = i;
				updateScrollOffset();
				seld = true;
				Sfx::CursorMove->play();
			}
			s.update(focus ? seld : false);
			if (seld)
			{
				if (left && s.callback)
				{
					SelPResp sr = s.callback(*this, s, Direction_Left);
					if (sr != SelPResp_Neutral) {
						s.animTimer.zero();
						Sfx::CursorSel->play();
						return; // Avoid iterator invalidation if callback cleared selections
					}
				}
				else if (right && s.callback)
				{
					SelPResp sr = s.callback(*this, s, Direction_Right);
					if (sr != SelPResp_Neutral) {
						s.animTimer.zero();
						Sfx::CursorSel->play();
						return; // Avoid iterator invalidation
					}
				}
				else if (((Kb::IsKeyFirstFrame(KB::Return) || Kb::IsKeyFirstFrame(KB::E)) || (Mouse::LeftRel && hovd && og)
					|| (Controller::BtnFrames[Btn_X] == 1) || Mouse::MiddleFrames == 1)
				)
				{
					if (s.callback) {
						SelPResp sr = s.callback(*this, s, Direction_Center);
						switch (sr)
						{
						case SelPResp_Neutral: {
							break;
						}
						default:
						{
							s.animTimer.zero();
							Sfx::CursorSel->play();
							return; // Avoid iterator invalidation
						}
						}
					}
					s.isClicked = true;
				}
			}
			if(mouseScrolled)
			{
				left = bleft;
				right = bright;
			}
			i++;
			it = itNext;
		}
	}
}

void SelectionPrompt::render(RendTarget* renderTarget)
{
	sf::Vector2f p(rectLeft, rectTop - scrollOffset);
	
	// Draw breadcrumb if enabled
	if (showBreadcrumb && !breadcrumb.empty())
	{
		sf::Text breadcrumbText(breadcrumb, *Fonts::MainFont, 20);
		breadcrumbText.setFillColor(sf::Color::Yellow);
		breadcrumbText.setOutlineColor(sf::Color::Black);
		breadcrumbText.setOutlineThickness(2);
		auto bounds = breadcrumbText.getLocalBounds();
		breadcrumbText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
		breadcrumbText.setPosition(rectLeft, rectTop - 30.0f);
		renderTarget->draw(breadcrumbText);
		
		// Adjust starting position for menu items to account for breadcrumb
		p.y += 20.0f;
	}
	
	if (vertical)
	{
		// Find the selected option's Y position
		float selectedY = rectTop - scrollOffset;
		float maxDistance = rectHeight + 50.0f; // Distance for full fade
		
		// Get the selected item's index
		int selectedIndex = 0;
		for (auto it = selections.begin(); it != selections.end(); ++it, ++selectedIndex) {
			if (it->index == (int)this->selectedIndex) break;
			selectedY += it->text.getLocalBounds().height * 1.25f;
		}
		
		for (auto& s : selections)
		{
			float itemHeight = s.text.getLocalBounds().height * 1.25f;
			float itemCenterY = p.y + itemHeight / 2.0f;

			// Calculate distance from item center to selected item center
			float distance = std::abs(itemCenterY - (selectedY + itemHeight/2.0f));
			
			// Calculate opacity based on distance from selected item
			float opacity = Util::Clamp(1.0f - (distance / maxDistance), 0.1f, 1.0f);
			
			auto fc = s.text.getFillColor();
			auto oc = s.text.getOutlineColor();
			
			fc.a = (sf::Uint8)(255 * opacity);
			oc.a = (sf::Uint8)(255 * opacity);
			s.text.setFillColor(fc);
			s.text.setOutlineColor(oc);
			s.text.setPosition(p);
			if (p.y + itemHeight >= rectTop && p.y <= rectTop + rectHeight)
			{
				renderTarget->draw(s.text);
			}
			p.y += itemHeight;
		}
	}
	else
	{
		p.x -= scrollOffset;
		float rectCenterX = rectLeft + rectWidth / 2.0f;
		float maxDistance = rectWidth / 2.0f + 50.0f; // Distance for full fade
		
		for (auto it = selections.begin(); it != selections.end(); ++it)
		{
			auto&s=*it;
			float itemWidth = s.text.getLocalBounds().width * 2;
			float itemCenterX = p.x + itemWidth / 2.0f;

			// Calculate distance from item center to rect center
			float distance = std::abs(itemCenterX - rectCenterX);
			
			// Calculate opacity based on distance (cylindrical fade)
			float opacity = Util::Clamp(1.0f - (distance / maxDistance), 0.1f, 1.0f);
			
			auto fc = s.text.getFillColor();
			auto oc = s.text.getOutlineColor();
			
			fc.a = (sf::Uint8)(255 * opacity);
			oc.a = (sf::Uint8)(255 * opacity);
			
			s.text.setFillColor(fc);
			s.text.setOutlineColor(oc);
			s.text.setPosition(p);
			renderTarget->draw(s.text/*, &G->fireShader->shader*/);
			p.x += itemWidth;
		}
	}
}

SelectionPrompt::Selection* SelectionPrompt::getSelected()
{
	if(selections.empty())
	{
		return nullptr;
	}
	auto selp = selections.begin();
	std::advance(selp, (int)Util::Clamp(selectedIndex, 0, selections.size() - 1));
	return &*selp;
}

void SelectionPrompt::updateScrollOffset()
{
	if (selections.empty()) return;
	
	// Calculate the position and height of the selected item
	float itemHeight = 0;
	float selectedItemY = 0;
	unsigned int i = 0;
	
	for (auto it = selections.begin(); it != selections.end(); ++it, ++i)
	{
		auto& s = *it;
		float currentItemHeight = s.text.getLocalBounds().height * 1.25f;
		
		if (i == selectedIndex)
		{
			selectedItemY = i * currentItemHeight;
			itemHeight = currentItemHeight;
			break;
		}
	}
	
	// Calculate visible area bounds
	float visibleTop = scrollOffset;
	float visibleBottom = scrollOffset + rectHeight;
	
	// Start smooth scroll animation
	scrollStartY = scrollOffset;
	if (selectedItemY < visibleTop)
	{
		scrollTargetY = selectedItemY;
	}
	else if (selectedItemY + itemHeight > visibleBottom)
	{
		scrollTargetY = selectedItemY + itemHeight - rectHeight;
	}
	else
	{
		scrollTargetY = scrollOffset;
	}
	
	// Calculate actual total height by summing all item heights
	float totalHeight = 0;
	for (auto& s : selections) {
		totalHeight += s.text.getLocalBounds().height * 1.25f;
	}
	// Clamp target scroll offset to valid range
	scrollTargetY = Util::Clamp(scrollTargetY, 0.0f, std::max(0.0f, totalHeight - rectHeight));
	scrollTimer.restart();
}

void SelectionPrompt::clearSelections()
{
	selections.clear();
	selectedIndex = 0;
	scrollOffset = 0.0f;
	scrollTargetY = 0.0f;
	scrollStartY = 0.0f;
}

void SelectionPrompt::setBreadcrumb(std::string breadcrumb)
{
	this->breadcrumb = breadcrumb;
	this->showBreadcrumb = !breadcrumb.empty();
}

std::string SelectionPrompt::getBreadcrumb() const
{
	return breadcrumb;
}

SelectionPrompt::Selection::Selection(SelectionPrompt& host, std::string ttext, bool enabled)
	:host(host)
	,callback(nullptr)
	,text(ttext, *Fonts::MainFont, 30)
	,animUp(false)
	,animMs(500)
	,enabled(enabled)
	,isClicked(false)
	,bef(nullptr)
	,next(nullptr)
	
	,fillColorR(255)
	,fillColorG(0)
	,fillColorB(255)

	,outlineColorR(255)
	,outlineColorG(255)
	,outlineColorB(255)

	,valFillColorR(255)
	,valFillColorG(0)
	,valFillColorB(0)

	,valOutlineColorR(0)
	,valOutlineColorG(255)
	,valOutlineColorB(0)

	,mouseScroll(Direction_None)
	,index(-1)
{
	text.setOutlineThickness(3);
	text.setOutlineColor(sf::Color{outlineColorR, outlineColorG, outlineColorB});
	valText.setFont(*Fonts::MainFont);
	valText.setCharacterSize(30);
	auto b = text.getLocalBounds();
	text.setOrigin(sf::Vector2f(b.width / 2, b.height / 2));
}

void SelectionPrompt::Selection::setValue(std::string val, 
	unsigned char fillColR, 
	unsigned char fillColG, 
	unsigned char fillColB, 

	unsigned char outlineColR, 
	unsigned char outlineColG, 
	unsigned char outlineColB
)
{
	valText.setString(val);
	valText.setOutlineThickness(3);
	valText.setFillColor({fillColR, fillColG, fillColB});
	valText.setOutlineColor({outlineColR, outlineColG, outlineColB});
	//auto b = text.getGlobalBounds();
}

void SelectionPrompt::Selection::setText(std::string text)
{
	this->text.setString(text);
}

void SelectionPrompt::Selection::update(bool isSelected)
{
	isClicked = false;

	animUp = !isSelected;

	animTimer.update2(animUp, animMs);

	float rms = Util::Clamp(animTimer.millis(), 0, animMs);
	float ams = Easing::Apply(isSelected ? Ease_OutElastic : Ease_In, rms, 0, animMs, animMs);

	float bottom = 1.0;
	float top = 1.1;
	float sc = Util::Scale(ams, 0, animMs, bottom, top);
	text.setScale(sf::Vector2f(sc, sc));
	auto col = sf::Color{fillColorR, fillColorG, fillColorB};
	col.a = Util::Scale(ams, 0, animMs, 100, 200);
	text.setFillColor(col);

	if (!isSelected)
	{
		text.setFillColor(sf::Color::Black);
	}

	//std::printf("%s: %llu\n", text.getString().getData(), animTimer.millis());
}

}