#include "Game/Kb.h"
#include "Game/Game.h"

namespace nyaa {

class GuiWidget* Kb::KeyFFGuiObj[KB::KeyCount];
unsigned int Kb::KeyHeldFrames[KB::KeyCount]; /*keyboard frames*/
unsigned int Kb::KeyReleased[KB::KeyCount]; /*keyboard release*/
std::map<unsigned int, float> Kb::KeysHeldMillis;
std::string Kb::LastEnteredText = "";

void Kb::Init()
{
	std::fill(std::begin(KeyHeldFrames), std::end(KeyHeldFrames), 0);
	std::fill(std::begin(KeyReleased), std::end(KeyReleased), 0);

	for(unsigned int i = 0; i < KB::KeyCount; ++i) 
	{
		KeysHeldMillis[i] = 0.0f;
		KeyFFGuiObj[i] = nullptr;
	}
}

bool Kb::IsKeyFirstFrame(unsigned int key)
{
	if (G->fadeState != Fade_Done || !G->winFocused) return false;
	return KeyHeldFrames[key] == 1;
}

bool Kb::IsKeyReleased(unsigned int key)
{
	if (G->fadeState != Fade_Done || !G->winFocused) return false;
	return KeyReleased[key] == 1;
}

bool Kb::IsKeyDown(unsigned int key)
{
	if (G->fadeState != Fade_Done || !G->winFocused) return false;
	return KeyHeldFrames[key];
}

float Kb::GetKeyMs(unsigned int key)
{
	if (G->fadeState != Fade_Done || !G->winFocused) return 0.0f;
	return KeysHeldMillis[key];
}

void Kb::ResetKeyMs(unsigned int key)
{
	KeysHeldMillis[key] = 0.0f;
}

}