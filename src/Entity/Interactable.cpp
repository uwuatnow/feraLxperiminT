#include "Entity/Prop.h"

namespace nyaa {

Interactable::~Interactable()
{
}

bool Interactable::onInteract(Actor* user)
{
	return true; //default true
}

} // namespace nyaa