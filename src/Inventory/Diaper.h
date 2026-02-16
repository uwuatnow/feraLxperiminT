#pragma once
#include "Game/Globals.h"
#include "Inventory/Equipment.h"
#include "Entity/Potty.h"

namespace nyaa {

class Diaper
	:public Equipment
	,public Potty
{
public:
	Diaper();

	virtual ~Diaper();
	
public:
	bool used;
};

}