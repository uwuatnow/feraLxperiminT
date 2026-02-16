#include "Game/OBB.h"
#include "Game/Util.h"

namespace nyaa {

void OBB::getCorners(Point corners[4]) const {
	float rad = angle * (Util::Pi / 180.0f);
	float cosA = cosf(rad);
	float sinA = sinf(rad);

	Point axes[2] = {
		{ cosA, sinA },
		{ -sinA, cosA }
	};

	corners[0] = center - axes[0] * halfExtents.x - axes[1] * halfExtents.y;
	corners[1] = center + axes[0] * halfExtents.x - axes[1] * halfExtents.y;
	corners[2] = center + axes[0] * halfExtents.x + axes[1] * halfExtents.y;
	corners[3] = center - axes[0] * halfExtents.x + axes[1] * halfExtents.y;
}

bool OBBIntersects(const OBB& a, const OBB& b, Point* mtv) {
	float radA = a.angle * (Util::Pi / 180.0f);
	float radB = b.angle * (Util::Pi / 180.0f);

	Point axes[4] = {
		{ cosf(radA), sinf(radA) },
		{ -sinf(radA), cosf(radA) },
		{ cosf(radB), sinf(radB) },
		{ -sinf(radB), cosf(radB) }
	};

	Point cornersA[4], cornersB[4];
	a.getCorners(cornersA);
	b.getCorners(cornersB);

	float minOverlap = 1e30f;
	Point smallestAxis;

	for (int i = 0; i < 4; i++) {
		Point axis = axes[i];
		
		float minA = 1e30f, maxA = -1e30f;
		for (int j = 0; j < 4; j++) {
			float p = cornersA[j].x * axis.x + cornersA[j].y * axis.y;
			minA = std::min(minA, p);
			maxA = std::max(maxA, p);
		}

		float minB = 1e30f, maxB = -1e30f;
		for (int j = 0; j < 4; j++) {
			float p = cornersB[j].x * axis.x + cornersB[j].y * axis.y;
			minB = std::min(minB, p);
			maxB = std::max(maxB, p);
		}

		if (maxA < minB || maxB < minA) return false;

		float overlap = std::min(maxA - minB, maxB - minA);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			smallestAxis = axis;
		}
	}

	if (mtv) {
		Point d;
		d.x = a.center.x - b.center.x;
		d.y = a.center.y - b.center.y;
		if (d.x * smallestAxis.x + d.y * smallestAxis.y < 0) smallestAxis = -smallestAxis;
		*mtv = smallestAxis * minOverlap;
	}

	return true;
}

bool OBBIntersectsLine(const OBB& obb, float p1X, float p1Y, float p2X, float p2Y, Point* mtv) {
	float rad = obb.angle * (Util::Pi / 180.0f);
	Point axes[3];
	axes[0] = { cosf(rad), sinf(rad) };
	axes[1] = { -sinf(rad), cosf(rad) };
	
	float dx = p2X - p1X;
	float dy = p2Y - p1Y;
	float len = sqrtf(dx * dx + dy * dy);
	if (len < 0.0001f) {
		axes[2] = { 0, 0 };
	} else {
		axes[2] = { -dy / len, dx / len }; // Line normal
	}

	Point corners[4];
	obb.getCorners(corners);
	Point linePoints[2] = { {p1X, p1Y}, {p2X, p2Y} };

	float minOverlap = 1e30f;
	Point smallestAxis;

	for (int i = 0; i < 3; i++) {
		Point axis = axes[i];
		if (axis.x == 0 && axis.y == 0) continue;

		float minA = 1e30f, maxA = -1e30f;
		for (int j = 0; j < 4; j++) {
			float p = corners[j].x * axis.x + corners[j].y * axis.y;
			minA = std::min(minA, p);
			maxA = std::max(maxA, p);
		}

		float minB = 1e30f, maxB = -1e30f;
		for (int j = 0; j < 2; j++) {
			float p = linePoints[j].x * axis.x + linePoints[j].y * axis.y;
			minB = std::min(minB, p);
			maxB = std::max(maxB, p);
		}

		if (maxA < minB || maxB < minA) return false;

		float overlap = std::min(maxA - minB, maxB - minA);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			smallestAxis = axis;
		}
	}

	if (mtv) {
		Point d;
		float midX = (p1X + p2X) * 0.5f;
		float midY = (p1Y + p2Y) * 0.5f;
		d.x = obb.center.x - midX;
		d.y = obb.center.y - midY;
		if (d.x * smallestAxis.x + d.y * smallestAxis.y < 0) smallestAxis = -smallestAxis;
		*mtv = smallestAxis * minOverlap;
	}

	return true;
}

Point OBBClosestPoint(const OBB& obb, const Point& externalPoint) {
	// Transform the external point into the OBB's local coordinate system
	float rad = obb.angle * (Util::Pi / 180.0f);
	float cosA = cosf(rad);
	float sinA = sinf(rad);

	// Vector from OBB center to external point
	Point d;
	d.x = externalPoint.x - obb.center.x;
	d.y = externalPoint.y - obb.center.y;

	// Transform to local space (rotate by -angle)
	Point localPoint;
	localPoint.x = d.x * cosA + d.y * sinA;
	localPoint.y = -d.x * sinA + d.y * cosA;

	// Clamp to the OBB's half-extents
	Point clampedLocal;
	clampedLocal.x = std::max(-obb.halfExtents.x, std::min(obb.halfExtents.x, localPoint.x));
	clampedLocal.y = std::max(-obb.halfExtents.y, std::min(obb.halfExtents.y, localPoint.y));

	// Transform back to world space
	Point closestPoint;
	closestPoint.x = obb.center.x + clampedLocal.x * cosA - clampedLocal.y * sinA;
	closestPoint.y = obb.center.y + clampedLocal.x * sinA + clampedLocal.y * cosA;

	return closestPoint;
}

}