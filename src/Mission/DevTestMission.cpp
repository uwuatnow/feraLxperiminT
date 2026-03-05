#include "Mission/DevTestMission.h"
#include "Entity/Actor.h"
#include "Screen/InGameScreen.h"
#include "Map/MapFactory.h"
#include "Entity/Door.h"
#include "Entity/DevCar.h"
#include "Entity/DealerCatgirl.h"
#include "Entity/Computer.h"
#include "Entity/Catgirl.h"
#include "Inventory/Gun.h"
#include "Inventory/GasCan.h"

namespace nyaa {

DevTestMission::DevTestMission()
	:Mission("DevTest 0001")
{
}

DevTestMission::~DevTestMission()
{
}

void DevTestMission::init()
{
	auto dev_test_map = IGS->mapFactory->get("dev_test");
	Actor* ca = IGS->player;
	ca->gotoMapInstant("dev_test");
	ca->setPosTile(-1, 4);
	ca->dirAngle = 90;
	Door* door = new Door();
	door->setPosTile(10, 4);
	dev_test_map->addEnt(door, true);

	//spawn a few cars
	for(int i = 0; i < 5; ++i)
	{
		DevCar* car = new DevCar();
		car->setPosTile(10 + (i * 2), 0);
		car->dirAngle = 90;
		dev_test_map->addEnt(car, true);
	}
	
  DealerCatgirl* dealer = new DealerCatgirl(
  255, 255, 255, 255,
  255, 255, 255, 255,
  255, 255, 255, 255,
  255, 255, 255, 255,
  255, 255, 255, 255,
  255, 255, 255, 255
  );
  dev_test_map->addEnt(dealer, true);
  dealer->setPosTile(-10, 6);

  // Setup home wandering behavior
  dealer->isHomeWandering = true;
  dealer->homePointX = dealer->posX;
  dealer->homePointY = dealer->posY;
  dealer->homeWalkRange = 80.0f; // Wander within 5 tiles (80 pixels)

	for(int i = 0; i < 5; i++)
	{
		Gun* gun = Gun::Create(GunType::Pistol);
		gun->setPosTile(-3 + (i * 2), -2);
		dev_test_map->addEnt(gun, true);
		
		Gun* gun2 = Gun::Create(GunType::Rifle);
		gun2->setPosTile(-3 + (i * 2), -4);
		dev_test_map->addEnt(gun2, true);
	}

	// Spawn gas cans
	for(int i = 0; i < 3; i++)
	{
		GasCan* gasCan = new GasCan();
		gasCan->setPosTile(-3 + (i * 2), -6);
		dev_test_map->addEnt(gasCan, true);
	}

	// Spawn computer
	Computer* computer = new Computer();
	computer->setPosTile(5, 5);
	dev_test_map->addEnt(computer, true);

	// Spawn 10 wandering npcs near -40x, -25y with 20 wander distance
	for(int i = 0; i < 10; ++i)
	{
		Catgirl* npc = new Catgirl();
		npc->setPosTile(-40 + (i % 5)* 2, -25 + (i / 5)* 2);
		dev_test_map->addEnt(npc, true);
		npc->isHomeWandering = true;
		npc->homePointX = npc->posX;
		npc->homePointY = npc->posY;
		npc->homeWalkRange = 320.0f; // 20 tiles * 16 pixels/tile
	}
}

}