#pragma once
#include "Game/Globals.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <string>
#include "Game/Timer.h"
#include "Texture/TextureMan.h"

namespace nyaa {

class HealthBar
{
public:
	HealthBar(std::string label, unsigned char colR, unsigned char colG, unsigned char colB);
	
public:
	void update();
	
	void render(float posX, float posY, RendTarget* renderTarget);

public:
	unsigned char colR, colG, colB;
	std::string label;
	float *realVal;
	float renderVal; /* start at 0 so it animates into the real val*/
	bool alphaUp;
	float alphaMs;
	Timer updateTimer;
	Timer alphaTimer;
	TextureMan::Tex& hbar;
	TextureMan::Tex& hbar_end;
	float alpha;
};


}