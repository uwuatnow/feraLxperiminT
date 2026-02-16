#pragma once
#include "Game/Globals.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <vector>
#include "Game/Enum.h"
#include "Game/Kb.h"
#include "Game/Clock.h"

namespace nyaa {
//
class Game
{
public:
	static const char* Title;
	static float FPSConstant;
	static const char* ResDir;
	static int AspectRatioL;
	static int AspectRatioR;
	static int ScreenWidth;
	static int ScreenHeight;

public:
	Game();
	
	~Game();

public:
	void ReallocInGameScreen();
	
	void close();

protected:
	void doLoop();
	
	void doEvents();
	
	void drawFades();

public:
	bool winResized;
	bool winFocused;
	bool isFullscreen;
	Fade fadeState;
	unsigned long long framesPassed;
	unsigned long long curScreenFramesPassed;
	float fpsLimit;
	// Frame delta time in milliseconds (deltaTime)
	float frameDeltaMillis;
	float lastTimeFps;
	float framesPerSecond;
	InputMethod inMethod;
	unsigned int joystickIndex;
	Clock KeysHeldClock;
private:
	sf::RenderWindow* win;
	sf::RenderTexture* gameWinRendTex;
	friend class ExitScreen;
	sf::View view;
public:
	Clock screenSwitchClock;
private:
	Clock initialFadeInClock;
public:
	class TVStaticShader* tvStaticShader;
public:
	Clock fpsClock;
	Clock frameDeltaClock;

#if DEBUG
	std::vector<class Sheet*> sheets;
#endif
	int selectedSaveSlot;
};

extern Game* G;

}
