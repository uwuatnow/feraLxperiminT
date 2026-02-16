#pragma once
#include "Game/Globals.h"
#include <string>
#include "Serialize/Serializable.h"

namespace nyaa {

class SaveData : public Serializable
{
public:
	SaveData(int slot);

	~SaveData();

public:
	void save();
	
	void load(std::string fileLoc);

public:
	int slot;
	
	bool firstMissionPassed;
};


}