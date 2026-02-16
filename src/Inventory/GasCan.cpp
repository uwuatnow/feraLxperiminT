#include "Inventory/GasCan.h"
#include "Inventory/Drug/PetrolTrip.h"
#include "Entity/Interaction.h"
#include "Entity/Actor.h"
#include "Game/DialogueBox.h"
#include "Screen/InGameScreen.h"
#include "Game/Sfx.h"

namespace nyaa {

GasCan::GasCan()
	:Item("Gas Can", "contains 50% corn ethanol", "gas_can", 1, 1, 0, 1, 1)
	,filled(true)
{
	interactions.push_back(new Interaction(this, "Huff", [](Interaction* in) -> bool
	{
		if(GasCan* gasCan = dynamic_cast<GasCan*>(in->e))
		{
			if(in->user)
			{
			if(gasCan->filled)
			{
				in->user->startTrip(new PetrolTrip());
				in->user->basedMeter += 3;
				in->user->hurt(10, Actor::DamageReason_DrugAbuse, in->e);
				Sfx::PlayRandomCough(1);
			}
				else
				{
					IGS->dbox.clear();
					IGS->dbox.add(in->user, "its empty");
					IGS->dbox.show();
				}
			}
		}
		else 
		{
			assert(0);
		}
		return true;
	}));
}

GasCan::~GasCan()
{
	//std::cout << "gas can dtor" << std::endl;
}

}