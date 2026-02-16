#pragma once
#include "Game/Globals.h"

#include <string>
#include <SFML/Graphics/Texture.hpp>
#include "Game/Timer.h"

namespace nyaa {

class Actor;

class WalkAnim
{
public:
	WalkAnim(std::string name, Actor* host);
	
public:
	void update();

public:
	Actor* host;
	unsigned int frame_time_ms;
	unsigned int index;
	
	sf::Texture tex;
	Timer timer;
	int frame;
	bool ping_pong;
	bool frame_incrementing;
	int columns;
	int rows;
	int stationary_frame;
};

}