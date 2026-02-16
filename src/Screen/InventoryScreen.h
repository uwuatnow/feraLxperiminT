#pragma once
#include "Game/Globals.h"
#include "Screen/Screen.h"

namespace nyaa {

class Item;

class InventoryScreen : public Screen
{
public:
	static InventoryScreen* Instance;

	InventoryScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;
	
	virtual bool onEnter() override;

	virtual void switchTo(bool noFadeTamper = false) override;

public:
	int m_SelectedSlot;
	int m_ScrollOffset;
	float m_ItemAnimTimer;
	float m_SelectionAnimOffset;

	static const int GRID_WIDTH;
	static const int GRID_HEIGHT;
	static const int SLOT_SIZE;
	static const int SLOT_SPACING;

	int getSelectedItemIndex() const;
	void drawGrid(RendTarget* renderTarget);
	void drawItemPreview(RendTarget* renderTarget) const;
	void handleGridInput();
	void handleMouseInput();
	int getSlotAtPosition(int posX, int posY) const;

	Item* m_DraggedItem;
	int m_DraggedFromIndex;
	int m_DragOffsetX, m_DragOffsetY;
};

}