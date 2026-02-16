#pragma once

namespace nyaa {

class ActorWellbeing
{
public:
    ActorWellbeing()
        :tiredness(0)
        ,dehydration(0)
        ,rage(0)
        ,tension(0)
        ,paranoia(0)
        ,paralysis(0)
    {

    }

public: //0.0 <-> 1.0
    float tiredness;
    float dehydration;
    float rage;
    float tension;
    float paranoia;
    float paralysis;
};

}
