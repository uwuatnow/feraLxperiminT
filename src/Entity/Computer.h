#pragma once
#include "Game/Globals.h"
#include "Entity/Prop.h"
#include <string>
#include <vector>
#include <SFML/System/Clock.hpp>
#include <random>

namespace nyaa {

class PCSite;

struct DesktopIcon
{
	std::string label;
	float posX, posY;
	bool dragging;
	float dragOffsetX, dragOffsetY;
	float lastClickTime;
};

struct DesktopWindow
{
	std::string title;
	float x, y, w, h;
	bool isOpen;
	bool dragging;
	float dragOffsetX, dragOffsetY;
	PCSite* site;
	float scrollY;
	float scale = 0.0f;
	bool isClosing = false;
	float renderX, renderY;
	std::vector<std::string> history;
	std::string currentLink;
};

class Computer : public Prop
{
public:
	enum State
	{
		State_Off,
		State_Booting,
		State_Desktop
	};

	Computer();

public:
	virtual bool onInteract(Actor* user) override;
	virtual void update() override;
	void shutdown();

public:
	State state;
	class Timer* bootTimer;

	std::vector<DesktopIcon> icons;
	std::vector<DesktopWindow> windows;
	sf::Clock desktopClock;
	
	// Random number generator for window position variation
	std::mt19937 windowPosRandomGen;
	std::uniform_real_distribution<float> windowPosRandomDist;
};

}