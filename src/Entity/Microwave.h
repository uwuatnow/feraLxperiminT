#pragma once
#include "Game/Globals.h"
#include "Entity/Prop.h"
#include "Game/Timer.h"
#include <SFML/Graphics/RenderTarget.hpp>

namespace nyaa {

class Item;

class Microwave : public Prop
{
public:
	Microwave();
	
public:
	virtual bool onInteract(Actor* user) override;
	
	bool IsCooked();
	
	void drawText(RendTarget& target);
	
	virtual void update() override;
	
	float getTimeRemaining();
	
public:
	float cookingTimeMillis;
	Timer timer;
	Item* itemInside;
};

}