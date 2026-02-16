#pragma once
#include "Game/Globals.h"
#include "Inventory/Drug/DrugTrip.h"

namespace nyaa {

class CannabisTrip : public DrugTrip
{
public:
    CannabisTrip(float sativa, float indica);
};

}