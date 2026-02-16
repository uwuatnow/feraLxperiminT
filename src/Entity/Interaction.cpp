#include "Entity/Interaction.h"
#include "Screen/InGameScreen.h"

namespace nyaa {

Interaction::Interaction(Entity* e, std::string name, InteractionCback func)
	:e(e)
	,name(name)
	,user(nullptr)
	,mission(nullptr)
	,func(func)
	,requireHoldMs(120.0f)
	,priorityLevel(0)
{
}

bool Interaction::call(Actor* user) 
{
	this->mission = IGS->curMission;
	this->user = user;
	return func(this);
}

Interaction::~Interaction() 
{
}

} // namespace nyaa