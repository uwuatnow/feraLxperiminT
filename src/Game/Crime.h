#pragma once
#include "Game/Globals.h"

namespace nyaa {

class Crime {
public:
    Crime();
    virtual ~Crime();

    virtual void update();

    unsigned int profile;

    float posX, posY;

    class Timer* timer;
};

} // namespace nyaa