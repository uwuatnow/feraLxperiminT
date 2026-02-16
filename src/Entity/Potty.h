#pragma once
#include "Game/Globals.h"
#include "Entity/Prop.h"

namespace nyaa {

class Potty : public Prop
{
public:
	Potty(double urineCapacity = 100.0f);
	
	virtual ~Potty();

public:
	virtual bool onInteract(Actor* user) override;
	
	void addUrine(double urineAmount);

	virtual void update() override;

public:
	double urineCapacity;
	double urineAmount;
};

}