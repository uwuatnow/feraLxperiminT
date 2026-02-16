#pragma once
#include "Game/Globals.h"
#include "Inventory/Item.h"

namespace nyaa {

class Equipment : public Item
{
public:
	Equipment(std::string name, std::string description);

	virtual ~Equipment();
};

}