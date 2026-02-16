#pragma once
#include "Game/Globals.h"
#include "Game/Clock.h"
#include "Entity/Prop.h"

namespace nyaa {

class Explosion : public Prop
{
public:
	Explosion();

	virtual ~Explosion();

public:
	Clock timer;
};

} // namespace nyaa