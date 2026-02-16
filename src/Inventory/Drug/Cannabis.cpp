#include "Inventory/Drug/Cannabis.h"
#include "Inventory/Drug/CannabisTrip.h"
#include "Entity/Actor.h"
#include "Game/Game.h"
#include "Inventory/Inventory.h"
#include "Game/Sfx.h"

namespace nyaa {

Cannabis::Cannabis() : Drug("canibis", "eeeeed", "cannabis", 1, 1, 0, 1, 1, Direction_Center)
{
    sativaRatio = 0.5f;  // Default 50/50 sativa/indica
    indicaRatio = 0.5f;
}

Cannabis::~Cannabis()
{
}

void Cannabis::use()
{
    if (inv && inv->owner) {
        Actor* actor = dynamic_cast<Actor*>(inv->owner);
        if (actor) {
            // Create a new cannabis trip and start it
            CannabisTrip* trip = new CannabisTrip(sativaRatio, indicaRatio);
            actor->startTrip(trip);
            //Util::PrintLnFormat("started a cannabis trip.");
        }
    }
    // Play cough sound effect
    Sfx::PlayRandomCough(0);
    // Mark for deletion after use
    markForDeletion();
}

}