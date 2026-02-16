#pragma once
#include "Game/Globals.h"

#include <vector>
#include "Game/Easing.h"
#include "Game/Clock.h"

namespace nyaa {

class Anim
{
public:
	class Kf
	{
	public:
		Kf();
		
	public:
		float posX, posY;
		float scaleX, scaleY;
		float rectLeft, rectTop, rectWidth, rectHeight;
		float rotation;
		unsigned char colR, colG, colB, colA;
		float alpha;
		float timeMilliseconds;
		bool visited;
		Ease easing;
	};

public:
	enum State
	{
		State_Paused, 
		State_Playing, 
		State_Done
	};

public:	
	Anim(float lengthMilliseconds,
		float startTimeMilliseconds = 0.0f,
		bool loop = false
	);
	
public:
	Kf& add();
	
	Kf& operator[](unsigned int index);
	
	Kf between();
	
	void update();
	
	void pause();
	
	void restart();
	
	void restart2(float atMillis);
	
	void stop();

public:
	float lengthMilliseconds;
	float currentTimeMilliseconds;
	bool loop;
	State state;
	unsigned int currentFrameIndex;
	std::vector<Kf> frames;
	Clock clock;
};

}