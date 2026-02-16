#include "Mission/FirstMission.h"
#include "Screen/InGameScreen.h"
#include "Entity/Microwave.h"
#include "Entity/Interaction.h"
#include "Entity/Fridge.h"
#include "Entity/Catgirl.h"
#include "Entity/Potty.h"
#include "Game/Util.h"
#include "Entity/Laptop.h"
#include "Game/SaveData.h"
#include "Inventory/HotPocket.h"
#include "Inventory/Shasta12ozCan.h"
#include "Screen/MissionFailedScreen.h"
#include "Inventory/Inventory.h"
#include "Game/Game.h"
#include "Game/Util.h"
#include "Map/MapFactory.h"
#include <iostream>
#include <cassert>
#include "Entity/Light.h"
#include "Entity/Road.h"
#include "Entity/Door.h"
#include "Inventory/Diaper.h"

namespace nyaa {

FirstMission::FirstMission()
	:Mission("Out of the basement")
	,mom(nullptr)
	,microwave(nullptr)
	,microwaveUsed(false)
	,pottyUsed(false)
{
}

FirstMission::~FirstMission()
{
}

void FirstMission::onEntityTick(Entity* e)
{
	if (e == mom)
	{
		//std::cout << std::format("mom\n");
	}
}

void FirstMission::doScreenStatsText(std::string& stats)
{
	if(microwave && microwave->itemInside)
	{
		auto timeLeft = microwave->getTimeRemaining();
		if(timeLeft > 0) stats += Util::Format("Time left:%.0fs\n", timeLeft);
	}
}

void FirstMission::init()
{
	Actor* ca = IGS->player;
	ca->gotoMapInstant("basement");
	ca->setPosTile(10, 5);
	ca->setFaceAngle(Entity::FaceAngle_TowardsCam);

	auto k = IGS->mapFactory->find("kitchen");
	//auto basement = IGS->mapFactory->find("basement");

	Potty* potty = new Potty();
	k->addEnt(potty, true);
	potty->setPosTile(1, -3);
	potty->setFaceAngle(Entity::FaceAngle_TowardsCam);

	Microwave* mw = new Microwave();
	mw->setFaceAngle(Entity::FaceAngle_TowardsCam);
	this->microwave = mw;
	auto mwi = new Interaction(mw, "Use microwave", [mw, this](Interaction* in) -> bool
	{
		auto& db = IGS->dbox;
		auto& inv = in->user->inv;
		HotPocket* hp = dynamic_cast<HotPocket*>(inv->equippedItem);
		if (
			hp
            ||
            mw->itemInside
		)
		{
			if(!this->microwaveUsed)
			{
                if(hp)
                {
                    hp->wrapped = false;
                }
                else assert(0);
			    if(mw->itemInside == nullptr) {
                        mw->itemInside = (Item*)inv->takeEquippedItem();
			    }
			    else assert(0);
				mw->cookingTimeMillis = 3000;
				mw->timer.reset();
				db.clear();
				db.add(in->user, "Come back in 30 seconds.");
				db.show();
				this->microwaveUsed = true;
			}
			else if(mw->IsCooked() && mw->itemInside)
			{
				db.clear();
				db.add(in->user, "Cooked hot pocket added to inventory");
                if(HotPocket* hp = dynamic_cast<HotPocket*>(mw->itemInside))
                {
                    hp->cooked = true;
                }
                else assert(0);
				in->user->inv->addAndEquip(mw->itemInside);
				mw->itemInside = nullptr;
				db.show();
			}
			else if(!mw->IsCooked() && mw->itemInside)
			{
				db.clear();
				db.add(in->user, "WAIT A MINUTE YOU DICK!");
				db.show();
			}
			else
            {
                /*db.clear();
				db.add(in->user, "wtf");
				db.show();*/
				//assert(0);
            }
		}
		else
		{
			db.clear();
			db.add(in->user, "You need to equip the hot pocket");			
			db.add(in->user, "Press I on your keyboard");
			db.show();
		}
		return true;
	});
	mwi->priorityLevel = 1;
	mw->interactions.push_back(mwi);
	k->addEnt(mw, true);
	mw->setPosTile(4, 0);

	auto fr = new Fridge();
	k->addEnt(fr, true);

	auto fi = new Interaction(fr, "Open fridge", [](Interaction* in) -> bool
	{
		DialogueBox& db = IGS->dbox;
		auto& objectiveIndex = IGS->curMission->objectiveIndex;
		if (objectiveIndex == 0)
		{
			db.clear();
			db.add(in->user, "You open the fridge and find lots of hot pockets and shasta.");
			db.add(in->user, "Shasta and hot pockets added to inventory.", [&](DialogueBox::Msg& m)
			{
				assert(m.actor);
				auto hp = new HotPocket();
				m.actor->inv->addItem(hp);
				auto sc = new Shasta12ozCan();
				m.actor->inv->addItem(sc);
				++objectiveIndex;
			});
			db.show();
			// in->user->hunger = 5;
			// in->user->bladder += 20;
			// in->user->energy = 100;
		}
		else
		{
			db.clear();
			db.add(in->user, "I think that's enough for now. Ugh.");
			db.show();
		}
		return true;
	});

	fr->interactions.push_back(fi);
	fi->requireHoldMs = 200;
	fr->setPosTile(10, 0);
	mom = new Catgirl(
		255, 255, 255, 255,
		0, 255, 0, 255,
		0, 0, 255, 255,
		0, 0, 0, 255,
		255, 255, 255, 255,
		255, 255, 255, 255
	);
	k->addEnt(mom, true);
	mom->setPosTile(10, 1);
	mom->setFaceAngle(Entity::FaceAngle_TowardsCam);
	auto talk2MomInter = new Interaction(mom, "Talk to Mom", [](Interaction* in) -> bool
	{
		auto& db = IGS->dbox;
		in->e->turnTowards(in->user);
		db.clear();
		db.add(in->user, "Why are you awake so late?"
			" You need to go to sleep so you're awake to get vaccinated tomorrow!"
			" Go to bed, or I'm taking away your computers again!"
		);
		db.show();
		return true;
	});
	talk2MomInter->priorityLevel = 1;
	mom->interactions.push_back(talk2MomInter);

	auto lt = new Laptop();
	k->addEnt(lt, true);
	lt->setPosTile(10, 1);
	lt->setFaceAngle(Entity::FaceAngle_AwayFromCam);

	auto b = IGS->mapFactory->find("basement");
	b->moveEntHere(ca, 10, 5);

	auto l = new Laptop();
	b->addEnt(l, true);
	l->setPosTile(ca->tilePosX - 2, ca->tilePosY);
	l->setFaceAngle(Entity::FaceAngle_Right);

	l = new Laptop();
	b->addEnt(l, true);
	l->setPosTile(ca->tilePosX + 2, ca->tilePosY);
	l->setFaceAngle(Entity::FaceAngle_Left);

	l = new Laptop();
	b->addEnt(l, true);
	l->setPosTile(ca->tilePosX, ca->tilePosY - 2);
	l->setFaceAngle(Entity::FaceAngle_TowardsCam);

	l = new Laptop();
	b->addEnt(l, true);
	l->setPosTile(ca->tilePosX, ca->tilePosY + 2);
	l->setFaceAngle(Entity::FaceAngle_AwayFromCam);

	ca->health = 88;
	ca->hunger = 76;
	ca->bladder = 60;

#if DEBUG
	ca->inv->addItem(new Diaper());
	//IM TIRED OF OPENING THE FRIDGE.
	auto hotP = new HotPocket();
	ca->inv->addAndEquip(hotP);
#endif
}

void FirstMission::onCameraSettle() {
	auto& dbox = IGS->dbox;
	auto& ca = IGS->player;
	dbox.clear();
	dbox.add(ca, "You find yourself in a basement surrounded by several computers. "
		"You're extremely hungry, maybe you should microwave a hot pocket!");
	dbox.add(ca, "Maybe you could find one in the kitchen? Hopefully your parents aren't "
		"awake so you can sneak upstairs and make one quickly.");
	dbox.show();
}

void FirstMission::failMission()
{
	DialogueBox& dbox = IGS->dbox;
	Actor* ca = IGS->player;
	if (failReason == FailReason_KilledUrMom)
	{
		dbox.clear();
		dbox.add(ca, "You killed your mom retard");
		dbox.closeCallback = []() -> void
		{
			MissionFailedScreen::Instance->switchTo();
		};
		dbox.show();
	}
	Mission::failMission();
}

void FirstMission::passMission()
{
	DialogueBox& dbox = IGS->dbox;
	Actor* ca = IGS->player;
	dbox.clear();
	dbox.add(ca, "You passed the first mission!");
	dbox.show();
	ca->inv->addItem(new Diaper());
	ca->inv->addItem(new HotPocket());
	ca->inv->addItem(new Shasta12ozCan());
	IGS->saveData->firstMissionPassed = true;
	Mission::passMission();
}

bool FirstMission::onActorDeath(Actor* a)
{
	if (a == mom)
	{
		failReason = FailReason_KilledUrMom;
		return true;
	}
	return false;
}

Mission::EventResponse FirstMission::onPropInteract(Prop* p, Actor* user)
{
	return EventResponse_Allow;
}

Mission* FirstMission::getNew()
{
	return new FirstMission();
}

}
