#pragma once
#include "Game/Globals.h"

namespace nyaa {

enum Ease : unsigned int
{
	Ease_None = 0,
	Ease_In = 1 << 0,
	Ease_Out = 1 << 1,
	Ease_InOut = 1 << 2,
	Ease_InCubic = 1 << 3,
	Ease_OutCubic = 1 << 4,
	Ease_InOutCubic = 1 << 5,
	Ease_InQuart = 1 << 6,
	Ease_OutQuart = 1 << 7,
	Ease_InOutQuart = 1 << 8,
	Ease_InQuint = 1 << 9,
	Ease_OutQuint = 1 << 10,
	Ease_InOutQuint = 1 << 11,
	Ease_InSine = 1 << 12,
	Ease_OutSine = 1 << 13,
	Ease_InOutSine = 1 << 14,
	Ease_InExpo = 1 << 15,
	Ease_OutExpo = 1 << 16,
	Ease_InOutExpo = 1 << 17,
	Ease_InCirc = 1 << 18,
	Ease_OutCirc = 1 << 19,
	Ease_InOutCirc = 1 << 20,
	Ease_InElastic = 1 << 21,
	Ease_OutElastic = 1 << 22,
	Ease_OutElasticHalf = 1 << 23,
	Ease_OutElasticQuarter = 1 << 24,
	Ease_InOutElastic = 1 << 25,
	Ease_InBack = 1 << 26,
	Ease_OutBack = 1 << 27,
	Ease_InOutBack = 1 << 28,
	Ease_InBounce = 1 << 29,
	Ease_OutBounce = 1 << 30,
	Ease_InOutBounce = static_cast<unsigned int>(1 << 31)
};

class Easing
{
public:
	static double Apply(Ease type, double val, double start, double end);
	
	static double ApplyClamped(Ease type, double val, double start, double end);
	
	static double Apply(Ease type, double time, double initial, double change, double duration);

private:
	static double ApplyOne(Ease type, double time, double initial, double change, double duration);
};

inline Ease operator|(Ease lhs, Ease rhs)
{
	return static_cast<Ease>(static_cast<int>(lhs) | static_cast<int>(rhs));
}


}