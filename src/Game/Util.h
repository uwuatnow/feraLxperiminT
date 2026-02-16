#pragma once
#include "Game/Globals.h"
#include "Game/Point.h"
#include <stdarg.h>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include "Game/Direction.h"

namespace nyaa {

class Util
{
public:
	static constexpr float Pi = 3.14159265f;
	
	static double ToRad(double deg) { return deg * (3.14159265358979323846 / 180.0); }
	static double ToDeg(double rad) { return rad * (180.0 / 3.14159265358979323846); }

	static std::random_device RandomDevice;
	static std::mt19937 mtGEN;
	static std::uniform_real_distribution<float> NormalizedDistro; //-1.0 - 1.0

public:
	template <typename T>
    static bool ContainsPointer(const std::vector<T*>& vec, T* ptr) {
        return std::find(vec.begin(), vec.end(), ptr) != vec.end();
    }

    template <typename T>
    static bool RemoveFromVec(std::vector<T>& vec, const T& value) {
        auto originalSize = vec.size();
        vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
        return vec.size() < originalSize;
    }

public:
	static double Round(double x);

	static void MakeDir(std::string dir);

	static bool DirExists(std::string dir);

	static bool FileExists(std::string file);

	static std::vector<std::string> Split(std::string str, char by);

	static bool StringVecContains(std::vector<std::string> &vec, std::string str);

	static Direction OppositeDirection(Direction dir);

	static Direction DirFromAngle(double angle);

	static double AngleFromDir(Direction dir);

	static std::string GetResLoc(std::string filename);

	static std::string GetPNGLoc(std::string filename);

	static std::string GetWAVLoc(std::string filename);

	static double Scale(double value, double from_min, double from_max, double to_min, double to_max);

	static double ScaleClamped(double value, double from_min, double from_max, double to_min, double to_max);

	static double Dist(double x1, double y1, double x2, double y2);

	static double Clamp(double val, double min, double max);

	static double Lowest(double v1, double v2);

	static double Max(double a, double b);

	static double RotateTowards(double fromX, double fromY, double toX, double toY);

	static void AngleLineRel(double fromX, double fromY, double deg, double dist, double* outX, double* outY);

	static void AngleLineAbs(double fromX, double fromY, double deg, double dist, double* outX, double* outY);

	static double SnapAngle(double angle);

	static double DistToLine(double pX, double pY, double p1X, double p1Y, double p2X, double p2Y);

	static int StringsAreEqual(const char* s1, const char* s2);

	static double AngleDifference(double angle1, double angle2);

	static bool Intersects(double aX, double aY, double bX, double bY, double cX, double cY, double dX, double dY);

	static bool GetPointWhereLinesIntersect(double aX, double aY, double bX, double bY, double cX, double cY, double dX, double dY, double* outX, double* outY);

	static double ReflectFromLine(double a_x, double a_y, double b_x, double b_y, double p_x, double p_y, double angle);

	static float RandNormalized();

	static double RepairAngle(double angle);

	static std::string Format(const char* fmt, ...);

	static void PrintLnFormat(const char* fmt, ...);

	static double getAngleBetweenVectors(double v1X, double v1Y, double v2X, double v2Y);
};

}