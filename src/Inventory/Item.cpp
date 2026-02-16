#include "Inventory/Item.h"
#include "Inventory/Inventory.h"
#include "Game/Game.h"
#include <cassert>
#include <iostream>
#include "Entity/Interaction.h"
#include "Map/Map.h"
#include "Entity/Actor.h"

namespace nyaa {

Item::Item(std::string name, std::string description, std::string texture, 
	unsigned int textureSizeFramesX, unsigned int textureSizeFramesY, 
	unsigned int defaultTextureFrameIndex, 
	unsigned int sizeX, unsigned int sizeY, 
	Direction anchor
)
	:Prop(texture, sizeX,sizeY, anchor)
	,defaultTextureFrameIndex(defaultTextureFrameIndex)
	,textureSizeFramesX(textureSizeFramesX)
	,textureSizeFramesY(textureSizeFramesY)
	,tex(nullptr)
	,inv(nullptr)
	,invSpriteOffsetX(0)
	,invSpriteOffsetY(0)
	,name(name)
	,description(description)
	,deleteMe(false)
{
	tex = &TextureMan::tman->get(texture.c_str());
	sp.setTexture(tex->tex, true);

	auto pickUpInter = new Interaction(this, "Pick up " + name, [](Interaction* in) -> bool
	{
		Item* item = (Item*)in->e;
		in->user->inv->addItem(item);
		item->hostMap->removeEnt(item);
		return true;
	});
	pickUpInter->priorityLevel = 2;
	interactions.push_back(pickUpInter);
}

Item::~Item()
{
	//assert(inv);
	//std::cout << "Item dtor" << std::endl;
}

void Item::equip()
{
	if (inv)
	{
		inv->equippedItem = this;
	}
}

void Item::use()
{

}

void Item::equippedUpdate()
{

}

void Item::markForDeletion()
{
    //assert(inv);
    //if(inv)
    //{
        //bool success = Util::RemoveFromVec(inv->items, this);
        //assert(success);

    //}
    assert(!deleteMe);
	//if(deleteMe){}
    deleteMe = true;
}

}
