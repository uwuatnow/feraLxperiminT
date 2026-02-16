#pragma once
#include "Game/Globals.h"
#include <string>
#include <vector>

namespace nyaa {

class Actor;
class Item;
class Serializable;
class InventoryScreen;

class Inv
{
public:
	Inv(Actor* owner);

	~Inv();

public:
    void update();

	void addItem(Item* item);

	Item* takeEquippedItem();

	void addAndEquip(Item* item);

	bool useFilledGasCan();

	bool hasFilledGasCan();

public:
	Actor* owner;
	std::string saveLoc;
	Item* equippedItem;
	int money;
	Serializable* serial;

private:
	std::vector<Item*> items;
	friend Actor;
	friend InventoryScreen;
	friend class InGameScreen;
};

}
