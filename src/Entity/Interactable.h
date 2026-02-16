#pragma once
#include "Game/Globals.h"

namespace nyaa {

class Actor;

class Interactable
{
public:
	//returns true if successful
	virtual bool onInteract(Actor* user);
	virtual ~Interactable();
};

} // namespace nyaa