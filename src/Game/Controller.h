#pragma once
#include "Game/Globals.h"
#include <map>

namespace nyaa {

enum Btn : unsigned int
{
	Btn_X = 0,
	Btn_Circle = 1,
	Btn_Square = 2,
	Btn_Triangle = 3,
	Btn_LBtn = 4,
	Btn_RBtn = 5,
	Btn_Select = 6, //share
	Btn_Start = 7, //options
};

class Controller
{
public:
    //controller dpad x
	static float dpx;

	//controller dpad y
	static float dpy; // positive is up negative is down

	//controller dpad: amount of frames held axis for
	static unsigned int dpxFrames;
	static unsigned int dpyFrames;

	//right stick
	static float rsX;
	static float rsY;

	//left trigger (LT) - used for aiming/locking player in place
	static float lt;

	static std::map<Btn, float> HeldMillis; //controller button milliseconds held for
	static std::map<Btn, unsigned int> BtnFrames; //amount of frames a controller button was held for
	static std::map<Btn, bool> BtnRel; //true if controller button was released
};

}