#pragma once
#include "Game/Globals.h"

namespace nyaa {

class Clock
{
public:
    Clock();
    ~Clock();
    Clock(const Clock& other);
    Clock& operator=(const Clock& other);
    
    long long getElapsedMicroseconds() const;
    long long getElapsedMilliseconds() const;
    double getElapsedSeconds() const;
    void restart();

private:
    long long microseconds;

    long long nowMicroseconds() const;
};

}