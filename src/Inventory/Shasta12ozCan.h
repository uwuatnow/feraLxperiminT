#pragma once
#include "Game/Globals.h"
#include "Inventory/Item.h"

namespace nyaa {

class Shasta12ozCan : public Item
{
public:
	Shasta12ozCan();
	
	virtual ~Shasta12ozCan();

public:
	bool cracked;
	bool empty;
};

}