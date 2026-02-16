#include "Game/Util.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <iostream>
#include <sstream>
#include "Game/Game.h"

namespace nyaa {

std::random_device Util::RandomDevice;
std::mt19937 Util::mtGEN(Util::RandomDevice());
std::uniform_real_distribution<float> Util::NormalizedDistro(-1.0f, 1.0f);

double Util::Round(double x)
{
    if (x >= 0)
	{
        return floor(x + 0.5);
    }
	else
	{
        return ceil(x - 0.5);
    }
}

void Util::MakeDir(std::string dir)
{
	CreateDirectoryA(dir.c_str(), NULL);
}

bool Util::DirExists(std::string dir)
{
	DWORD attribs = GetFileAttributesA(dir.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES) && (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

bool Util::FileExists(std::string file)
{
	DWORD attribs = GetFileAttributesA(file.c_str());
	return attribs != INVALID_FILE_ATTRIBUTES;
}

std::vector<std::string> Util::Split(std::string str, char by)
{
	std::stringstream ss(str);
	std::vector<std::string> ret;
	while (ss.good())
	{
		std::string sub;
		std::getline(ss, sub, by);
		ret.push_back(sub);
	}
	return ret;
}

bool Util::StringVecContains(std::vector<std::string>& vec, std::string str)
{
	for(auto& s : vec) 
	{
		if (s == str)
		{
			return true;
		}
	}
	return false;
}

double Util::Max(double a, double b)
{
	return a > b ? a : b;
}

Direction Util::OppositeDirection(Direction dir)
{
	switch (dir)
	{
	case Direction_Up: return Direction_Down;
	case Direction_Down: return Direction_Up;
	case Direction_Left: return Direction_Right;
	case Direction_Right: return Direction_Left;
	//case Direction_North: return Direction_South;
	//case Direction_South: return Direction_North;
	//case Direction_West: return Direction_East;
	//case Direction_East: return Direction_West;
	//case Direction_Top: return Direction_Bottom;
	//case Direction_Bottom: return Direction_Top;
	case Direction_Center: return Direction_Center;
	default: return dir; /* idk how the fuck this could happen */
	}
}

double Util::AngleFromDir(Direction dir)
{
	unsigned int d = (unsigned int)dir;
	return d == 0 ? 270.0 : d == 1 ? 90.0 : d == 2 ? 180.0 : d == 3 ? 0.0 : 0.0;
}

Direction Util::DirFromAngle(double angle)
{
	//angle += 0.6;
	double a = fabs(angle < 0 ? 180.0 + (360.0 - (180.0 - angle)) : angle);
	if (a >= 0 && a < 45) return Direction_Right;
	if (a >= 45 && a < 135) return Direction_Down;
	if (a >= 135 && a < 225) return Direction_Left;
	if (a >= 225 && a < 315) return Direction_Up;
	if (a >= 315 && a <= 360) return Direction_Right;

	return Direction_Right;
}

std::string Util::GetResLoc(std::string filename)
{
	return std::string(Game::ResDir) + filename;
}

std::string Util::GetPNGLoc(std::string filename)
{
	return std::string(Game::ResDir) + filename + ".png";
}

std::string Util::GetWAVLoc(std::string filename)
{
	return std::string(Game::ResDir) + filename + ".wav";
}

double Util::Scale(double value, double from_min, double from_max, double to_min, double to_max)
{
	return ((to_max - to_min) * (value - from_min) / (from_max - from_min)) + to_min;
}

double Util::ScaleClamped(double value, double from_min, double from_max, double to_min, double to_max)
{
	return Util::Clamp(Util::Scale(value, from_min, from_max, to_min, to_max), to_min, to_max);
}

double Util::Dist(double x1, double y1, double x2, double y2)
{
	double nmx = (x1 - x2);
	double nmy = (y1 - y2);
	return sqrt((nmx * nmx) + (nmy * nmy));
}

double Util::Lowest(double v1, double v2)
{
	return (v1 < v2 ? v1 : (v2 < v1 ? v2 : v1));
}

double Util::RotateTowards(double fromX, double fromY, double toX, double toY)
{
	return RepairAngle(atan2(toY - fromY, toX - fromX) * 180.0 / 3.14159265358979323846);
}

double Util::Clamp(double val, double min, double max)
{
	return val < min ? min : (val > max ? max : val);
}

int Util::StringsAreEqual(const char* s1, const char* s2)
{
	return (strcmp(s1, s2) == 0);
}

void Util::AngleLineRel(double fromX, double fromY, double deg, double dist, double* outX, double* outY)
{
	double radians = (deg / 360.0) * (3.14159265358979323846 * 2.0);
	if (outX) *outX = (fromX + cos(radians) * dist);
	if (outY) *outY = (fromY + sin(radians) * dist);
}

void Util::AngleLineAbs(double fromX, double fromY, double deg, double dist, double* outX, double* outY)
{
	double radians = (deg / 360.0) * (3.14159265358979323846 * 2.0);
	if (outX) *outX = cos(radians) * dist;
	if (outY) *outY = sin(radians) * dist;
}

double Util::SnapAngle(double angle)
{
	return (double)((int)(angle / 45.0) * 45);
}

double Util::DistToLine(double pX, double pY, double p1X, double p1Y, double p2X, double p2Y)
{
	double A = pX - p1X; // position of point rel one end of line
	double B = pY - p1Y;
	double C = p2X - p1X; // vector along line
	double D = p2Y - p1Y;
	double E = -D; // orthogonal vector
	double F = C;

	double dot = A * E + B * F;
	double len_sq = E * E + F * F;

	return fabs(dot) / sqrt(len_sq);
}

double Util::AngleDifference(double angle1, double angle2)
{
	double diff = fmod((angle2 - angle1 + 180.0), 360.0) - 180.0;
	return fabs(diff < -180.0 ? diff + 360.0 : diff);
}

bool ccw(double aX, double aY, double bX, double bY, double cX, double cY)
{
	return (cY - aY) * (bX - aX) > (bY - aY) * (cX - aX);
}

bool ccw(Point A, Point B, Point C)
{
	return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
}

bool UtilIntersects(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D)
{
	return ccw((double)A.x, (double)A.y, (double)C.x, (double)C.y, (double)D.x, (double)D.y) != ccw((double)B.x, (double)B.y, (double)C.x, (double)C.y, (double)D.x, (double)D.y) && 
           ccw((double)A.x, (double)A.y, (double)B.x, (double)B.y, (double)C.x, (double)C.y) != ccw((double)A.x, (double)A.y, (double)B.x, (double)B.y, (double)D.x, (double)D.y);
}

bool Util::Intersects(double aX, double aY, double bX, double bY, double cX, double cY, double dX, double dY)
{
	return ccw(aX, aY, cX, cY, dX, dY) != ccw(bX, bY, cX, cY, dX, dY) && ccw(aX, aY, bX, bY, cX, cY) != ccw(aX, aY, bX, bY, dX, dY);
}

double Util::RepairAngle(double angle)
{
	return fabs(angle < 0 ? 180.0 + (360.0 - (180.0 - angle)) : angle);
}

std::string Util::Format(const char* fmt, ...)
{
	const size_t BUFFER_SIZE = 2048;
	char buffer[BUFFER_SIZE];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, BUFFER_SIZE, fmt, args);
	va_end(args);
	return {buffer};
}

void Util::PrintLnFormat(const char* fmt, ...)
{
	const size_t BUFFER_SIZE = 2048;
	char buffer[BUFFER_SIZE];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, BUFFER_SIZE, fmt, args);
	va_end(args);
	std::cout << buffer << std::endl;
}

bool Util::GetPointWhereLinesIntersect(double aX, double aY, double bX, double bY, double cX, double cY, double dX, double dY, double* outX, double* outY)
{
	if (!Intersects(aX, aY, bX, bY, cX, cY, dX, dY)) return false;

	// Line AB represented as a1x + b1y = c1
	double a1 = bY - aY;
	double b1 = aX - bX;
	double c1 = a1 * (aX) + b1 * (aY);

	// Line CD represented as a2x + b2y = c2
	double a2 = dY - cY;
	double b2 = cX - dX;
	double c2 = a2 * (cX) + b2 * (cY);

	double determinant = a1 * b2 - a2 * b1;

	if (determinant == 0)
	{
		return false;
	}
	else
	{
		if (outX) *outX = (b2 * c1 - b1 * c2) / determinant;
		if (outY) *outY = (a1 * c2 - a2 * c1) / determinant;
		return true;
	}
}

double Util::ReflectFromLine(double a_x, double a_y, double b_x, double b_y, double p_x, double p_y, double angle)
{
	double radians = (angle / 360.0) * (3.14159265358979323846 * 2.0);
	double relative_b_x = b_x - a_x;
	double relative_b_y = b_y - a_y;
	double line_scaler = 1.0 / sqrt(relative_b_x * relative_b_x + relative_b_y * relative_b_y);

	double direction_x = cos(radians);
	double direction_y = sin(radians);
	double unit_x = relative_b_y * line_scaler;
	double unit_y = -(relative_b_x * line_scaler);

	double reflection_x = direction_x - (2.0 * (unit_x * direction_x + unit_y * direction_y) * unit_x);
	double reflection_y = direction_y - (2.0 * (unit_x * direction_x + unit_y * direction_y) * unit_y);
	double deg = atan2(reflection_y, reflection_x);

	return ((deg / (3.14159265358979323846 * 2.0)) * 360.0);
}

float Util::RandNormalized()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

double Util::getAngleBetweenVectors(double v1X, double v1Y, double v2X, double v2Y) {
	double dot = v1X * v2X + v1Y * v2Y;
	double v1_len = sqrt(v1X * v1X + v1Y * v1Y);
	double v2_len = sqrt(v2X * v2X + v2Y * v2Y);
	double cos_angle = dot / (v1_len * v2_len);
	return acos(std::max(-1.0, std::min(1.0, cos_angle)));
}

}
