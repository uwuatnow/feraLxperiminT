#pragma once
#include "Game/Globals.h"
#include "Game/GuiWidget.h"

namespace nyaa {

struct CollisionLinePoint { double x, y; };

class CollisionLine : public GuiWidget
{
public:
	CollisionLine();
	
	CollisionLine(double p1X, double p1Y, double p2X, double p2Y);
	
public:
	friend bool operator==(const CollisionLine& c1, const CollisionLine& c2);

public:
	CollisionLinePoint p1;
	CollisionLinePoint p2;
};

}