#pragma once
#include "Screen/Screen.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "Texture/Sheet.h"
#include "Game/SelectionPrompt.h"
#include <vector>

namespace nyaa {

class MenuScreen : public Screen
{
public:
	MenuScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;
	
	virtual bool onEnter() override;

public:
	SelectionPrompt sp;
	bool showCredits;

	static MenuScreen* Instance;
};

}