#pragma once
#include "Game/Globals.h"
#include "Inventory/Drug/DrugEffectQuantum.h"
#include <map>

namespace nyaa {

class DrugTrip
{
public:
    DrugTrip();
    
    virtual ~DrugTrip();

    virtual void update();
    
    float getLengthSecs();
    
    class Timer* tripTimer;
    bool finished;
    float lengthSecs;
protected:
    void appendToTimeline(float atSecond, DrugEffectQuantum effect);
private:
    std::map<float /* Seconds */, DrugEffectQuantum> tripTimeline;
};

}
