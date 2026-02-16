#pragma once
#include "Game/Globals.h"
#include "Inventory/Item.h"

namespace nyaa {

class HotPocket : public Item
{
public:
	HotPocket();

	virtual ~HotPocket();

public:
	bool wrapped;
	bool cooked;
	Interaction* eatInteraction;
};

}
