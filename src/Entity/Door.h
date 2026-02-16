#pragma once
#include "Game/Globals.h"
#include "Entity/Entity.h"

namespace nyaa {

class Door : public Entity
{
public:
	Door();

	virtual void update() override;
};

} // namespace nyaa