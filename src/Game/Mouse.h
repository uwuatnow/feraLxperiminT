#pragma once
#include "Game/Globals.h"

namespace nyaa {

class Mouse
{
public:	
	//mouse position
	static int Pos_X;
	static int Pos_Y;

	//mouse position (on click frame 0)
	static int PosOC_X;
	static int PosOC_Y;

	static unsigned int LeftFrames;
	static bool LeftRel;
	static unsigned int RightFrames;
    static bool RightRel;
    static bool Moved;
    static unsigned int MiddleFrames;
    static bool MiddleRel;
    static int WheelDelta;
};

}