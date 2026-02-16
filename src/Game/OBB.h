#pragma once
#include "Game/Point.h"

namespace nyaa {

struct OBB
{
    Point center;
    Point halfExtents;
    float angle; // in degrees

    void getCorners(Point corners[4]) const;
};

bool OBBIntersects(const OBB& a, const OBB& b, Point* mtv = nullptr);
bool OBBIntersectsLine(const OBB& obb, float p1X, float p1Y, float p2X, float p2Y, Point* mtv = nullptr);

// Find the closest point on the OBB's surface to a given external point
Point OBBClosestPoint(const OBB& obb, const Point& externalPoint);

}