#pragma once
#include "Game/Globals.h"
#include "Inventory/Item.h"

namespace nyaa {

class GasCan : public Item
{
public:
	GasCan();
	
	virtual ~GasCan();

public:
	bool filled;
};

}