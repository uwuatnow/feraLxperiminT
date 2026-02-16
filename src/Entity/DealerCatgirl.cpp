#include "Entity/DealerCatgirl.h"
#include "Inventory/Item.h"
#include "Game/DialogueBox.h"
#include "Entity/Interaction.h"
#include "Inventory/Inventory.h"
#include "Inventory/Gun.h"
#include "Screen/InGameScreen.h"
#include "Entity/BulletProjectile.h"
#include "Entity/DevCar.h"
#include "Game/EventScheduler.h"
#include "Game/Util.h"

namespace nyaa {

DealerCatgirl::DealerCatgirl(
	unsigned char bodyColR,
	unsigned char bodyColG,
	unsigned char bodyColB,
	unsigned char bodyColA,

	unsigned char eyeColR,
	unsigned char eyeColG,
	unsigned char eyeColB,
	unsigned char eyeColA,

	unsigned char hairColR,
	unsigned char hairColG,
	unsigned char hairColB,
	unsigned char hairColA,

	unsigned char clothesColR,
	unsigned char clothesColG,
	unsigned char clothesColB,
	unsigned char clothesColA,

	unsigned char tailColR,
	unsigned char tailColG,
	unsigned char tailColB,
	unsigned char tailColA,

	unsigned char outlineColR,
	unsigned char outlineColG,
	unsigned char outlineColB,
	unsigned char outlineColA
)
	:Catgirl(
		bodyColR, bodyColG, bodyColB, bodyColA,
		eyeColR, eyeColG, eyeColB, eyeColA,
		hairColR, hairColG, hairColB, hairColA,
		clothesColR, clothesColG, clothesColB, clothesColA,
		tailColR, tailColG, tailColB, tailColA,
		outlineColR, outlineColG, outlineColB, outlineColA
	)
	,botherCount(0)
{
	auto sellQuery = new Interaction(this, "Ask \"What are you selling?\"", [](Interaction* in) -> bool
	{
		if(DealerCatgirl* dealer = dynamic_cast<DealerCatgirl*>(in->e))
		{
			auto& db = IGS->dbox;
            db.clear();
			dealer->turnTowards(IGS->player);
            if(dealer->botherCount == 0)
            {
				dealer->botherCount++;
                db.add(dealer, "I'm selling nothing rn sorry.");
                db.add(dealer, "Come back later!");
            }
            else if (dealer->botherCount == 1)
            {
				dealer->botherCount++;

                db.add(dealer, "go away");
                dealer->inv->addAndEquip(Gun::Create(GunType::Pistol));
            }
            else if (dealer->botherCount == 2)
            {
                db.add(dealer, "ok thats it", [dealer](DialogueBox::Msg& m) {					
					EventScheduler::GlobalScheduler->scheduleEvent(1000, [dealer]() {
						dealer->botherCount++;
					});
				});
            }
			db.show();
		}
		return true;
	});
	sellQuery->priorityLevel = 2;
    interactions.push_back(sellQuery);
}

void DealerCatgirl::update()
{
    Catgirl::update();
    if(botherCount >= 2)
    {
        // Face the player and aim
        if(IGS->player && IGS->player->isControllable && IGS->player->health > 0)
        {
            // Set direction to face the player
            turnTowards(IGS->player);
            
            // If we have a gun equipped, shoot at the player
            if(botherCount >= 3 && inv->equippedItem && dynamic_cast<Gun*>(inv->equippedItem))
            {
                // Use the gun to shoot
                inv->equippedItem->use();
				//Util::PrintLnFormat("Dealer catgirl shot at player");
            }
        }
    }
}

void DealerCatgirl::hurt(float amount, DamageReason damageReason, Entity* damageSource)
{
	//Util::PrintLnFormat("Dealer catgirl hurt");
    Catgirl::hurt(amount, damageReason, damageSource);
	Entity* realDmgSrc = damageSource;
	if(BulletProjectile* bullet = dynamic_cast<BulletProjectile*>(damageSource))
	{
		realDmgSrc = bullet->shooter;
	}
	if(CarBase* car = dynamic_cast<CarBase*>(damageSource))
	{
		realDmgSrc = car->occupant;
	}
    if(realDmgSrc == IGS->player)
	{
		auto gun = Gun::Create(GunType::Pistol);
		inv->equippedItem = gun;
		inv->addItem(gun);
		botherCount = 3;
	}
}

} // namespace nyaa