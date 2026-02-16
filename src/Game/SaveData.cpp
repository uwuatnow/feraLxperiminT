#include "Game/SaveData.h"
#include <fstream>
#include "Game/Util.h"
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include <cassert>
#include "Inventory/Inventory.h"

namespace nyaa {

SaveData::SaveData(int slot)
	:Serializable("SaveFile", nullptr, Util::Format("./saves/" "%d" ".json", slot)) //is root serializable obj
	,slot(slot)
	,firstMissionPassed(false)
{
	assert(slot >= 0);
	//assert(slot < 4);
	//Util::PrintLnFormat("Loading save #%d/4", slot + 1);
	
	NYAA_SERIAL_ADD(this, firstMissionPassed,);

	Read();
}

SaveData::~SaveData()
{
}

void SaveData::save()
{	
	Util::PrintLnFormat("Saving to slot #%d/4", slot + 1);
	IGS->player->inv->serial->upload();
	upload();
}

void SaveData::load(std::string fileLoc)
{
}

}
