#include "Entity/Laptop.h"
#include "Screen/InGameScreen.h"
#include "Game/Util.h"
#include "Entity/Interaction.h"
#include "Entity/Actor.h"
#include "Mission/Mission.h"

namespace nyaa {

Laptop::Laptop()
	:Prop("laptop", 2, 2)
{
	friction = 2.0f;
	interactions.push_back(new Interaction(this, "Use laptop", [](Interaction* in) -> bool
	{
		auto& db = IGS->dbox;
		if (IGS->curMission->onEntityInteract(in->e) == Mission::EventResponse_NotImplemented)
		{
			if (in->user->getDir() == Util::OppositeDirection(in->e->getDir()))
			{
				in->user->isControllable = false;
				in->user->dirAngle = Util::AngleFromDir(Util::OppositeDirection(in->e->getDir()));
				db.clear();
				db.add(in->user, "You open the web browser and browse 4chan.");
				db.add(in->user, "Maybe I should watch some anime and listen to some lolicore next?");
				db.show();
			}
			else
			{
				db.clear();
				db.add(in->user, "You need to be facing the screen to use it, idiot.");
				db.show();
			}
		}
		return true;
	}));
	dirAngle = 90;
}

bool Laptop::onInteract(Actor* user)
{
	return true;
}

}