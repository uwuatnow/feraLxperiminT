#pragma once
#include "Game/Globals.h"
#include "Entity/Entity.h"
#include <vector>

namespace nyaa {

struct RoadVertex {
	struct { double x, y, z; } position;
	struct { double x, y, z; } normal;
	double texCoordX, texCoordY;
};

struct RoadMesh {
	std::vector<RoadVertex> vertices;
	std::vector<unsigned int> indices;
};

class Road : public Entity
{
public:
	Road();
	
public:
	RoadMesh generateCatmullRoadMesh(int segments = 64) const;

public:
	struct AnchorPos { int x, y;  };
	std::vector<AnchorPos> anchors;
	double width;
	int laneCount;
	unsigned char asphaltColorR, asphaltColorG, asphaltColorB;
	unsigned char linesColorR, linesColorG, linesColorB;

private:
	void catmullRom(
		const double p0X, const double p0Y,
		const double p1X, const double p1Y,
		const double p2X, const double p2Y,
		const double p3X, const double p3Y,
		double t,
		double* outX, double* outY) const;
	RoadMesh roadMesh;
};

} // namespace nyaa