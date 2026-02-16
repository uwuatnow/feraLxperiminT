#include "Game/WorldCrime.h"
#include "Game/Crime.h"
#include "Game/Timer.h"

namespace nyaa {

WorldCrime::WorldCrime()
{
}

WorldCrime::~WorldCrime()
{
    for (auto crime : crimes) {
        delete crime;
    }
}

void WorldCrime::update()
{
    for (auto crime : crimes) {
        crime->update();
    }
}

void WorldCrime::addCrime(class Crime* crime)
{
    crimes.push_back(crime);
}

} // namespace nyaa