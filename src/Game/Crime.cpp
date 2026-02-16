#include "Game/Crime.h"
#include "Game/Timer.h"

namespace nyaa {

Crime::Crime()
    :profile(0)
    ,posX(0)
    ,posY(0)
{
    timer = new Timer();
}

Crime::~Crime()
{
    delete timer;
}

void Crime::update()
{
    timer->update();
}

} // namespace nyaa