#pragma once
#include "Game/Globals.h"
#include <map>
#include <SFML/Window/Keyboard.hpp>

namespace nyaa {

using KB = sf::Keyboard;

class Kb
{
private:    
    static unsigned int KeyHeldFrames[KB::KeyCount]; /*keyboard frames*/
    static unsigned int KeyReleased[KB::KeyCount]; /*keyboard release*/
    static std::map<unsigned int, float> KeysHeldMillis;
    static class GuiWidget* KeyFFGuiObj[KB::KeyCount];
    friend class Game;

public:
    static void Init();

	static bool IsKeyFirstFrame(unsigned int key);
	
	static bool IsKeyReleased(unsigned int key);

	static bool IsKeyDown(unsigned int key);
	
	static void ResetKeyMs(unsigned int key);

	static float GetKeyMs(unsigned int key);	

	static std::string LastEnteredText;
};

}