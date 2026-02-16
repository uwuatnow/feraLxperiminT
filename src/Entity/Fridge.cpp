#include "Entity/Fridge.h"
#include "Entity/Interaction.h"
#include <cassert>
#include "Game/Sfx.h"

namespace nyaa {

Fridge::Fridge()
	:Prop("fridge", 1, 1, Direction_Bottom)
	,open(false)
{
	originX = 0.5f;
	originY = 1.0f;
	friction = 0.06f;

	// interactions.push_back(new Interaction(this, "Open/close fridge", [](Interaction* in) -> bool
	// {
	// 	if(Fridge* fridge = dynamic_cast<Fridge*>(in->e))
	// 	{
	// 		fridge->open = !fridge->open;
	// 		if(fridge->open) Sfx::CarDoorOpen->play(fridge->posX, fridge->posY);
	// 		else Sfx::CarDoorClose->play(fridge->posX, fridge->posY);
	// 	}
	// 	else 
	// 	{
	// 		assert(0);
	// 	}
	// 	return true;
	// }));
}

void Fridge::update()
{
	//dir = Direction_Down;
	//dirAngle = 90;
	Prop::update();
}

} // namespace nyaa