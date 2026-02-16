#pragma once
#include "Game/Globals.h"
#include <string>
#include <functional>

namespace nyaa {

typedef std::function<bool(class Interaction*)> InteractionCback;

class Interaction 
{
public:
	Interaction(class Entity* e, std::string name, InteractionCback func);

	virtual ~Interaction();

public:
	bool call(class Actor* user);

public:
	class Entity* e;
	std::string name;
	class Actor* user;
	class Mission* mission;
	InteractionCback func;
	float requireHoldMs; //0 = instant activation with E
	int priorityLevel;
};

}