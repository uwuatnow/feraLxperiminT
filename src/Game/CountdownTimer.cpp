#include "Game/CountdownTimer.h"

namespace nyaa {

CountdownTimer::CountdownTimer(float secondsRemaining)
  :secondsRemaining(secondsRemaining)
{
  
}

CountdownTimer::~CountdownTimer()
{
  
}

void CountdownTimer::update()
{
  timer.update();
}

}
