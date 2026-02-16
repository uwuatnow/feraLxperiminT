#pragma once
#include "Game/Globals.h"
#include <vector>

namespace nyaa {

class WorldCrime {
public:
    WorldCrime();
        
    ~WorldCrime();

public:
    void update();
    
    void addCrime(class Crime* crime);

private:
    std::vector<class Crime*> crimes;
};

} // namespace nyaa