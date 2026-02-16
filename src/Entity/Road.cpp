#include "Entity/Road.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>

namespace nyaa {

Road::Road()
	:width(32.0)
	,laneCount(2)
	,asphaltColorR(64)
	,asphaltColorG(64)
	,asphaltColorB(64)
	,linesColorR(255)
	,linesColorG(255)
	,linesColorB(0)
{
	flags |= EntFlag_UnderAllOtherEnts;
}

void Road::catmullRom(
	const double p0X, const double p0Y,
	const double p1X, const double p1Y,
	const double p2X, const double p2Y,
	const double p3X, const double p3Y,
	double t,
	double* outX, double* outY) const
{
	double t2 = t * t;
	double t3 = t2 * t;

	if(outX) *outX = 0.5 * ((2.0 * p1X) +
		(-p0X + p2X) * t +
		(2.0 * p0X - 5.0 * p1X + 4.0 * p2X - p3X) * t2 +
		(-p0X + 3.0 * p1X - 3.0 * p2X + p3X) * t3);

	if(outY) *outY = 0.5 * ((2.0 * p1Y) +
		(-p0Y + p2Y) * t +
		(2.0 * p0Y - 5.0 * p1Y + 4.0 * p2Y - p3Y) * t2 +
		(-p0Y + 3.0 * p1Y - 3.0 * p2Y + p3Y) * t3);
}

RoadMesh Road::generateCatmullRoadMesh(int segments) const
{
	RoadMesh mesh;
	if (anchors.size() < 2) return mesh;
	
	for (int i = 0; i < (int)anchors.size() - 1; ++i)
	{
		sf::Vector2f p0 = i > 0 ? sf::Vector2f(static_cast<float>(anchors[i-1].x * 16.0), static_cast<float>(anchors[i-1].y * 16.0)) : sf::Vector2f(static_cast<float>(anchors[i].x * 16.0), static_cast<float>(anchors[i].y * 16.0));
		sf::Vector2f p1 = sf::Vector2f(static_cast<float>(anchors[i].x * 16.0), static_cast<float>(anchors[i].y * 16.0));
		sf::Vector2f p2 = sf::Vector2f(static_cast<float>(anchors[i+1].x * 16.0), static_cast<float>(anchors[i+1].y * 16.0));
		sf::Vector2f p3 = i < (int)anchors.size() - 2 ? sf::Vector2f(static_cast<float>(anchors[i+2].x * 16.0), static_cast<float>(anchors[i+2].y * 16.0)) : sf::Vector2f(static_cast<float>(anchors[i+1].x * 16.0), static_cast<float>(anchors[i+1].y * 16.0));
		
		
		for (int s = 0; s <= segments; ++s)
		{
			// Skip first vertex of subsequent anchor pairs to avoid duplicates
			if (i > 0 && s == 0) continue;

			double t = static_cast<double>(s) / segments;
			sf::Vector2f center{};
			double centerX, centerY;
			catmullRom(static_cast<double>(p0.x), static_cast<double>(p0.y), static_cast<double>(p1.x), static_cast<double>(p1.y), static_cast<double>(p2.x), static_cast<double>(p2.y), static_cast<double>(p3.x), static_cast<double>(p3.y), t, &centerX, &centerY);
			center.x = static_cast<float>(centerX);
			center.y = static_cast<float>(centerY);
			
			sf::Vector2f nextPoint{};
			if (s < segments) {
					double nextPointX, nextPointY;
					catmullRom(static_cast<double>(p0.x), static_cast<double>(p0.y), static_cast<double>(p1.x), static_cast<double>(p1.y), static_cast<double>(p2.x), static_cast<double>(p2.y), static_cast<double>(p3.x), static_cast<double>(p3.y), t + 0.001, &nextPointX, &nextPointY);
					nextPoint.x = static_cast<float>(nextPointX);
					nextPoint.y = static_cast<float>(nextPointY);
				} else {
					double nextPointX, nextPointY;
					catmullRom(static_cast<double>(p0.x), static_cast<double>(p0.y), static_cast<double>(p1.x), static_cast<double>(p1.y), static_cast<double>(p2.x), static_cast<double>(p2.y), static_cast<double>(p3.x), static_cast<double>(p3.y), t - 0.001, &nextPointX, &nextPointY);
					nextPoint.x = static_cast<float>(nextPointX);
					nextPoint.y = static_cast<float>(nextPointY);
			}
			
			sf::Vector2f dir;
			if (s < segments) {
				dir = nextPoint - center;
			} else {
				dir = center - nextPoint;
			}
			
			float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
			if (len > 0) dir /= len;
			
			sf::Vector2f perp(-dir.y, dir.x);
			sf::Vector2f left = center + perp * static_cast<float>(width * 0.5);
			sf::Vector2f right = center - perp * static_cast<float>(width * 0.5);
			
			unsigned int baseIdx = (unsigned int)mesh.vertices.size();
			RoadVertex v1 = {{static_cast<double>(left.x), 0.0, static_cast<double>(left.y)}, {0.0, 1.0, 0.0}, 0.0, static_cast<double>(i) + t};
			RoadVertex v2 = {{static_cast<double>(right.x), 0.0, static_cast<double>(right.y)}, {0.0, 1.0, 0.0}, 1.0, static_cast<double>(i) + t};
			mesh.vertices.push_back(v1);
			mesh.vertices.push_back(v2);
			
			if (baseIdx >= 2)
			{
				mesh.indices.push_back(baseIdx-2);
				mesh.indices.push_back(baseIdx-1);
				mesh.indices.push_back(baseIdx);
				mesh.indices.push_back(baseIdx-1);
				mesh.indices.push_back(baseIdx+1);
				mesh.indices.push_back(baseIdx);
			}
		}
	}
	
	return mesh;
}

} // namespace nyaa