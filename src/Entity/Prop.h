#pragma once
#include "Game/Globals.h"
#include "Entity/Entity.h"
#include "Interactable.h"
#include "Game/Direction.h"

namespace nyaa {

class Prop
	:public Entity
	,public Interactable
{
public:
	Prop(std::string name,
		unsigned sizeX = 1, unsigned sizeY = 1,
		Direction anchor = Direction_Center,
		bool dontLoadSheet = false
	);

	virtual ~Prop();

private:
	void initType();
	
public:
	class Sheet* sh;
	Direction anchor;
};

}