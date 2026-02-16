#include "Game/Timer.h"
#include "Game/Util.h"

namespace nyaa {

Timer::Timer()
	:microseconds(0)
	,updateFrame(0)
	,restartThresholdMs(100)
	,resetOnFirstUse(true)
	,lengthMs(0)
	,ease(Ease_None)
{

}

Timer::Timer(float startMilliseconds, float restartThresholdMs, 
	bool resetOnFirstUse, float lengthMs, Ease ease
)
	:microseconds(startMilliseconds * 1000)
	,updateFrame(0)
	,restartThresholdMs(restartThresholdMs)
	,resetOnFirstUse(resetOnFirstUse)
	,lengthMs(lengthMs)
	,ease(ease)
{

}

void Timer::update()
{
	update2(false, 0);
}

void Timer::update2(bool minus, long long int msLimit) 
{
	//if (updateFrame != 0 && G->framesPassed == updateFrame)return;
	//updateFrame = G->framesPassed;

	if (resetOnFirstUse)
	{
		reset();
		resetOnFirstUse = false;
		return;
	}

	long long ms = clock.getElapsedMicroseconds();

	if (ms >= restartThresholdMs * 1000)
	{
		clock.restart();
		return;
	}

	if (!minus && msLimit > 0 && (microseconds + ms >= msLimit * 1000))
	{
		microseconds = msLimit * 1000;
		clock.restart();
		return;
	}

	if (minus && ((long long int) microseconds) - ms <= 0)
	{
		microseconds = 0;
		clock.restart();
		return;
	}

	if (minus)
	{
		microseconds -= ms;
	}
	else
	{
		microseconds += ms;
	}
	//std::printf("added: %lldms\n", ms);
	clock.restart();
}

void Timer::reset(float startMilliseconds)
{
	//std::printf("timer reset\n");
	microseconds = startMilliseconds * 1000.0f;
	clock.restart();
}

float Timer::secs()
{
	return millis() / 1000.0f;
}

float Timer::millis()
{
	if (ease != Ease_None && lengthMs > 0)
	{
		return Easing::Apply(ease, Util::Clamp(microseconds / 1000.0f, 0, lengthMs), 0, lengthMs);
	}
	else
	{
		return microseconds / 1000.0f;
	}
}

void Timer::setMillis(float milliseconds)
{
	microseconds = milliseconds * 1000.0f;
}

void Timer::zero()
{
	microseconds = 0;
}

bool Timer::once(float milliseconds)
{
	if (millis() >= milliseconds)
	{
		reset();
		return true;
	}
	return false;
}

}