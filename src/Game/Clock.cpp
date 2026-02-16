#include "Game/Clock.h"
#include <chrono>

namespace nyaa {

Clock::Clock()
    :microseconds(0)
{
    restart();
}

Clock::~Clock()
{
}

Clock::Clock(const Clock& other)
    :microseconds(other.microseconds)
{
}

Clock& Clock::operator=(const Clock& other)
{
    if (this != &other)
    {
        microseconds = other.microseconds;
    }
    return *this;
}

long long Clock::getElapsedMicroseconds() const
{
    return nowMicroseconds() - microseconds;
}

long long Clock::getElapsedMilliseconds() const
{
    return getElapsedMicroseconds() / 1000;
}

double Clock::getElapsedSeconds() const
{
    return getElapsedMicroseconds() / 1000000.0;
}

void Clock::restart()
{
    microseconds = nowMicroseconds();
}

long long Clock::nowMicroseconds() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

}