#include "Entity/Microwave.h"
#include "Game/Util.h"
#include "Game/Game.h"
#include "Inventory/Item.h"
#include <SFML/Graphics/Text.hpp>

namespace nyaa {

Microwave::Microwave()
	:Prop("microwave", 4, 1)
	,cookingTimeMillis(0)
	,itemInside(nullptr)
{
	friction = 0.1f;
}

bool Microwave::onInteract(Actor* user)
{
	return false;
}

bool Microwave::IsCooked()
{
	return cookingTimeMillis != 0 && timer.millis() >= cookingTimeMillis;
}

void Microwave::update()
{
	Entity::update();
	timer.update();
	dirAngle = 90;
}

void Microwave::drawText(sf::RenderTarget &target)
{	
	/* if(cookingTimeMillis != 0)
	{
		sf::Text timeLeftText(Util::Format("%.2fs remaining", getTimeRemaining()), 
			*Fonts::OSDFont);
		timeLeftText.setCharacterSize(5);
		target.draw(timeLeftText);
	} */
}

float Microwave::getTimeRemaining()
{
	return (cookingTimeMillis - Util::Clamp(timer.millis(), 0, cookingTimeMillis)) / 1000.0f;
}

}