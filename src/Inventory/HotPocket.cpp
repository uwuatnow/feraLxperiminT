#include "Inventory/HotPocket.h"
#include "Entity/Interaction.h"
#include "Screen/InGameScreen.h"
#include "Game/DialogueBox.h"
#include "Entity/Actor.h"
#include "Mission/FirstMission.h"
#include <cassert>

namespace nyaa {

HotPocket::HotPocket()
	:Item("Hot pocket", "Causes 5th degree burns", "hotpocket", 4, 1, Direction_UpLeft)
	,wrapped(true)
	,cooked(false)
	,eatInteraction(new Interaction(this, "Eat hot pocket", [this](Interaction* in) -> bool
    {
		DialogueBox & db = IGS -> dbox;
        
        if(FirstMission* fm = dynamic_cast<FirstMission*>(IGS->curMission))
        {
            fm->pass();
        }
        else
        {
            db.clear();
            db.add(in->user, "mmm yummy :3");
            //db.add(in->user, "test");
            db.show();
        }
        //else assert(0);
		
        assert(in->user);
        if(Actor* a = dynamic_cast<Actor*>(in->user))
        {
            a->hunger = 0;
            a->health = 100;
			a->energy = 100;
            a->bladder += 25;
        }
        //else assert(0);
        this->markForDeletion();
        return true;
    }))
{

}

HotPocket::~HotPocket()
{
    delete eatInteraction;
}

}
