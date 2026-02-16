#include "Easing.h"
#include "Game/Util.h"
#include <cmath>

namespace nyaa {

double Easing::Apply(Ease type, double val, double start, double end)
{
	return Apply(type, val, start, end, end);
}

double Easing::ApplyClamped(Ease type, double val, double start, double end)
{
	return Util::Clamp(Apply(type, val, start, end, end), start, end);
}

double Easing::Apply(Ease type, double time, double initial, double change, double duration) {
	double all = 0.0;
	int c = 0;
	for (int i = 0; i < 32; i++) {
		if ((int)type & (1 << i)) {
			all += ApplyOne(type, time, initial, change, duration);
			c++;
		}
	}
	return all / c;
}

double Easing::ApplyOne(Ease type, double time, double initial, double change, double duration) {
	if (change == 0 || time == 0 || duration == 0) return initial;
	if (time == duration) return initial + change;

	const float pi = 3.14159274f;
	switch (type)
	{
	default:
		return change * (time / duration) + initial;
	case Ease_None:
		return change;
	case Ease_In:
		return change * (time /= duration) * time + initial;
	case Ease_Out:
		return -change * (time /= duration) * (time - 2) + initial;
	case Ease_InOut:
		if ((time /= duration / 2) < 1) return change / 2 * time * time + initial;
		return -change / 2 * ((--time) * (time - 2) - 1) + initial;
	case Ease_InCubic:
		return change * (time /= duration) * time * time + initial;
	case Ease_OutCubic:
		return change * ((time = time / duration - 1) * time * time + 1) + initial;
	case Ease_InOutCubic:
		if ((time /= duration / 2) < 1) return change / 2 * time * time * time + initial;
		return change / 2 * ((time -= 2) * time * time + 2) + initial;
	case Ease_InQuart:
		return change * (time /= duration) * time * time * time + initial;
	case Ease_OutQuart:
		return -change * ((time = time / duration - 1) * time * time * time - 1) + initial;
	case Ease_InOutQuart:
		if ((time /= duration / 2) < 1) return change / 2 * time * time * time * time + initial;
		return -change / 2 * ((time -= 2) * time * time * time - 2) + initial;
	case Ease_InQuint:
		return change * (time /= duration) * time * time * time * time + initial;
	case Ease_OutQuint:
		return change * ((time = time / duration - 1) * time * time * time * time + 1) + initial;
	case Ease_InOutQuint:
		if ((time /= duration / 2) < 1) return change / 2 * time * time * time * time * time + initial;
		return change / 2 * ((time -= 2) * time * time * time * time + 2) + initial;
	case Ease_InSine:
		return -change * cos(time / duration * (pi / 2)) + change + initial;
	case Ease_OutSine:
		return change * sin(time / duration * (pi / 2)) + initial;
	case Ease_InOutSine:
		return -change / 2 * (cos(pi * time / duration) - 1) + initial;
	case Ease_InExpo:
		return change * pow(2, 10 * (time / duration - 1)) + initial;
	case Ease_OutExpo:
		return (time == duration) ? initial + change : change * (-pow(2, -10 * time / duration) + 1) + initial;
	case Ease_InOutExpo:
		if ((time /= duration / 2) < 1) return change / 2 * pow(2, 10 * (time - 1)) + initial;
		return change / 2 * (-pow(2, -10 * --time) + 2) + initial;
	case Ease_InCirc:
		return -change * (sqrt(1 - (time /= duration) * time) - 1) + initial;
	case Ease_OutCirc:
		return change * sqrt(1 - (time = time / duration - 1) * time) + initial;
	case Ease_InOutCirc:
		if ((time /= duration / 2) < 1) return -change / 2 * (sqrt(1 - time * time) - 1) + initial;
		return change / 2 * (sqrt(1 - (time -= 2) * time) + 1) + initial;
	case Ease_InElastic:
	{
		if ((time /= duration) == 1) return initial + change;

		double p = duration * .3;
		double a = change;
		double s = 1.70158;
		if (a < fabs(change)) { a = change; s = p / 4; }
		else s = p / (2 * pi) * asin(change / a);
		return -(a * pow(2, 10 * (time -= 1)) * sin((time * duration - s) * (2 * pi) / p)) + initial;
	}
	case Ease_OutElastic:
	{
		if ((time /= duration) == 1) return initial + change;

		double p = duration * .3;
		double a = change;
		double s = 1.70158;
		if (a < fabs(change)) { a = change; s = p / 4; }
		else s = p / (2 * pi) * asin(change / a);
		return a * pow(2, -10 * time) * sin((time * duration - s) * (2 * pi) / p) + change + initial;
	}
	case Ease_OutElasticHalf:
	{
		if ((time /= duration) == 1) return initial + change;

		double p = duration * .3;
		double a = change;
		double s = 1.70158;
		if (a < fabs(change)) { a = change; s = p / 4; }
		else s = p / (2 * pi) * asin(change / a);
		return a * pow(2, -10 * time) * sin((0.5f * time * duration - s) * (2 * pi) / p) + change + initial;
	}
	case Ease_OutElasticQuarter:
	{
		if ((time /= duration) == 1) return initial + change;

		double p = duration * .3;
		double a = change;
		double s = 1.70158;
		if (a < fabs(change)) { a = change; s = p / 4; }
		else s = p / (2 * pi) * asin(change / a);
		return a * pow(2, -10 * time) * sin((0.25f * time * duration - s) * (2 * pi) / p) + change + initial;
	}
	case Ease_InOutElastic:
	{
		if ((time /= duration / 2) == 2) return initial + change;

		double p = duration * (.3 * 1.5);
		double a = change;
		double s = 1.70158;
		if (a < fabs(change)) { a = change; s = p / 4; }
		else s = p / (2 * pi) * asin(change / a);
		if (time < 1) return -.5 * (a * pow(2, 10 * (time -= 1)) * sin((time * duration - s) * (2 * pi) / p)) + initial;
		return a * pow(2, -10 * (time -= 1)) * sin((time * duration - s) * (2 * pi) / p) * .5 + change + initial;
	}
	case Ease_InBack:
	{
		double s = 1.70158;
		return change * (time /= duration) * time * ((s + 1) * time - s) + initial;
	}
	case Ease_OutBack:
	{
		double s = 1.70158;
		return change * ((time = time / duration - 1) * time * ((s + 1) * time + s) + 1) + initial;
	}
	case Ease_InOutBack:
	{
		double s = 1.70158;
		if ((time /= duration / 2) < 1) return change / 2 * (time * time * (((s *= (1.525)) + 1) * time - s)) + initial;
		return change / 2 * ((time -= 2) * time * (((s *= (1.525)) + 1) * time + s) + 2) + initial;
	}
	case Ease_InBounce:
		return change - ApplyOne(Ease_OutBounce, duration - time, 0, change, duration) + initial;
	case Ease_OutBounce:
		if ((time /= duration) < (1 / 2.75))
		{
			return change * (7.5625 * time * time) + initial;
		}
		else if (time < (2 / 2.75))
		{
			return change * (7.5625 * (time -= (1.5 / 2.75)) * time + .75) + initial;
		}
		else if (time < (2.5 / 2.75))
		{
			return change * (7.5625 * (time -= (2.25 / 2.75)) * time + .9375) + initial;
		}
		else
		{
			return change * (7.5625 * (time -= (2.625 / 2.75)) * time + .984375) + initial;
		}
	case Ease_InOutBounce:
		if (time < duration / 2) return ApplyOne(Ease_InBounce, time * 2, 0, change, duration) * .5 + initial;
		return ApplyOne(Ease_OutBounce, time * 2 - duration, 0, change, duration) * .5 + change * .5 + initial;
	}
}

}