#include "Map/CollisionLine.h"

namespace nyaa {

CollisionLine::CollisionLine()
	:p1{ 0, 0 }
	,p2{ 0, 0 }
{

}

CollisionLine::CollisionLine(double p1X, double p1Y, double p2X, double p2Y)
	:p1{ p1X, p1Y }
	,p2{ p2X, p2Y }
{
	
}

bool operator==(const CollisionLine& c1, const CollisionLine& c2) 
{
	return c1.p1.x == c2.p1.x && c1.p1.y == c2.p1.y && c1.p2.x == c2.p2.x && c1.p2.y == c2.p2.y;
}

}
