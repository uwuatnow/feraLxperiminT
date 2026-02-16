#pragma once
#include "Game/Globals.h"
#include "Entity/Prop.h"

namespace nyaa {

class Laptop : public Prop
{
public:
	Laptop();
	
public:
	virtual bool onInteract(Actor* user) override;
};

}