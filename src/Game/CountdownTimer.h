#pragma once
#include "Game/Globals.h"
#include "Game/Timer.h"

namespace nyaa {

class CountdownTimer
{
public:
  CountdownTimer(float secondsRemaining);
  
  ~CountdownTimer();
public:

  void update();
  
public:
  Timer timer;
  float secondsRemaining;
};

}
