#include "Inventory/Equipment.h"

namespace nyaa {

Equipment::Equipment(std::string name, std::string description)
	:Item(name, description)
{
}

Equipment::~Equipment()
{
}

}