#include "Inventory/Inventory.h"
#include "Inventory/Item.h"
#include "Inventory/GasCan.h"
#include "Game/Game.h"
#include "Screen/InGameScreen.h"
#include "Serialize/Serializable.h"
#include "Game/SaveData.h"
#include "Game/Util.h"
#include <iostream>

namespace nyaa {

Inv::Inv(Actor* owner)
	:owner(owner)
	,equippedItem(nullptr)
	,money(0)
	,serial(new Serializable("inv", IGS->saveData, ""))
{
	if(!IGS->player)
    {
        serial->add("money", money);
	    serial->Read();
    }
}

Inv::~Inv()
{
    delete serial;
	for(Item* i : items)
	{
		delete i;
	}
}

void Inv::update()
{
    for(auto it = items.begin(); it != items.end();)
    {
        if((*it)->deleteMe)
        {
            if(*it == equippedItem) equippedItem = nullptr;
#if DEBUG
            //G->sheets.clear();
#endif // DEBUG
            delete *it;
            it = items.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void Inv::addItem(Item* item)
{
    if(!item) return;
    if(Util::ContainsPointer(items, item))
    {
        std::cout << "some1 tried to add dupe item!!" << std::endl;
        return;
    }
	item->inv = this;
	items.push_back(item);
}

Item* Inv::takeEquippedItem()
{
    if(!equippedItem) return nullptr;
    int idx = 0;
    for(auto i : items)
    {
        if(i == equippedItem)
        {
            items.erase(items.begin() + idx);
            break;
        }
        idx++;
    }
    Item* ret = equippedItem;
    equippedItem = nullptr;
    return ret;
}

void Inv::addAndEquip(Item* item)
{
    if(!item)
    {
        return;
    }
    addItem(item);
    equippedItem = item;
}

bool Inv::useFilledGasCan()
{
    for (auto it = items.begin(); it != items.end(); ++it) {
        GasCan* gasCan = dynamic_cast<GasCan*>(*it);
        if (gasCan && gasCan->filled) {
            gasCan->filled = false;
            return true;
        }
    }
    return false;
}

bool Inv::hasFilledGasCan()
{
    for (auto item : items) {
        GasCan* gasCan = dynamic_cast<GasCan*>(item);
        if (gasCan && gasCan->filled) {
            return true;
        }
    }
    return false;
}

} // namespace nyaa
