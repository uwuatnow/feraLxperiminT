#include "Mission/KillEvilCatgirlsMission.h"
#include "Screen/InGameScreen.h"
#include "Inventory/Gun.h"
#include "Entity/Catgirl.h"
#include "Inventory/Inventory.h"
#include "Map/MapFactory.h"

namespace nyaa {

KillEvilCatgirlsMission::KillEvilCatgirlsMission()
	:Mission("Kill Evil catgirls!") 
{
}

void KillEvilCatgirlsMission::init()
{
	IGS->player->inv->addAndEquip(Gun::Create(GunType::Pistol));
	auto b = IGS->mapFactory->find("basement");
	for (int i = 0; i < 2; i++) 
	{
		auto bc = new Catgirl(
			80, 60, 50, 255,
			50, 50, 50, 255,
			15, 15, 15, 255,
			40, 80, 150, 255,
			15, 15, 15, 255,
			255, 255, 255, 255
		);
		b->addEnt(bc, true);
		//bc->boolTags["is_evil"] = true;
		auto gun = Gun::Create(GunType::Pistol);
		bc->inv->addItem(gun);
		bc->follow = IGS->player;
		gun->equip();
		bc->setPosTile(10 + i, 12);
	}
}

bool KillEvilCatgirlsMission::onActorDeath(Actor* a) 
{
	auto ame = IGS->getAllMissionEnts();
	return !ame.empty();
}

void KillEvilCatgirlsMission::doScreenStatsText(std::string& stats)
{
}

}