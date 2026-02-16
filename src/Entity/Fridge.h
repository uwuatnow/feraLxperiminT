#pragma once
#include "Game/Globals.h"
#include "Entity/Prop.h"

namespace nyaa {

class Fridge : public Prop
{
public:
	Fridge();

	virtual void update() override;

	bool open;
};

} // namespace nyaa