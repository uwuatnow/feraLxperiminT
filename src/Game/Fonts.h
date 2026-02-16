#pragma once
#include "Game/Globals.h"
#include <SFML/Graphics/Font.hpp>

namespace nyaa {

class Fonts
{
public:
	static FontDataSt* MainFont;
	static FontDataSt* MonoFont;
	static FontDataSt* OSDFont;
	
	static void Init();
	static void Cleanup();
};

}