#include "Game/Mouse.h"

namespace nyaa {

//mouse position
int Mouse::Pos_X = 0;
int Mouse::Pos_Y = 0;

//mouse position (on click frame 0)
int Mouse::PosOC_X = 0;
int Mouse::PosOC_Y = 0;

unsigned int Mouse::LeftFrames = 0;
bool Mouse::LeftRel = false;
unsigned int Mouse::RightFrames = 0;
bool Mouse::RightRel = false;
bool Mouse::Moved = false;
unsigned int Mouse::MiddleFrames = 0;
bool Mouse::MiddleRel = false;
int Mouse::WheelDelta = 0;

}