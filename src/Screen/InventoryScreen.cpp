#include "Screen/InventoryScreen.h"
#include "Game/Game.h"
#include <SFML/Graphics/Text.hpp>
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include "Game/Util.h"
#include "Inventory/Item.h"
#include "Inventory/HotPocket.h"
#include "Inventory/Diaper.h"
#include "Inventory/GasCan.h"
#include "Entity/Interaction.h"
#include "Map/Map.h"
#include "Inventory/Inventory.h"
#include "Game/Fonts.h"
#include "Game/Mouse.h"
#include <SFML/Graphics/CircleShape.hpp>
#include "Game/Sfx.h"
#include "Game/Controller.h"

namespace nyaa {

InventoryScreen* InventoryScreen::Instance = nullptr;

#define GRID_START 70

const int InventoryScreen::GRID_WIDTH = 9;
const int InventoryScreen::GRID_HEIGHT = 4;
const int InventoryScreen::SLOT_SIZE = 48;
const int InventoryScreen::SLOT_SPACING = 4;

InventoryScreen::InventoryScreen()
	:m_SelectedSlot(0)
	,m_ScrollOffset(0)
	,m_ItemAnimTimer(0.f)
	,m_SelectionAnimOffset(0.f)
	,m_DraggedItem(nullptr)
	,m_DraggedFromIndex(-1)
	,m_DragOffsetX(0)
	,m_DragOffsetY(0)
{
	Instance = this;
}

void InventoryScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);
	if (Kb::IsKeyReleased(KB::Escape) || Kb::IsKeyReleased(KB::Q) || Controller::BtnFrames[Btn_Triangle] == 1)
	{
		myLastScreen->switchTo();
	}

	// Controls hint
	sf::Text controls("[E] Equip  [X] Drop  [Q] Exit", *Fonts::OSDFont, 16);
	controls.setFillColor(sf::Color(180, 180, 180));
	controls.setPosition(70, 10);
	renderTarget->draw(controls);

	// Update animations
	m_ItemAnimTimer += 0.02f;

	// Handle input
	handleGridInput();
	handleMouseInput();

	// Draw grid
	drawGrid(renderTarget);

	// Draw dragged item
	if(m_DraggedItem && m_DraggedItem->tex)
	{
		m_DraggedItem->sp.setPosition(Mouse::Pos_X + m_DragOffsetX, Mouse::Pos_Y + m_DragOffsetY);
		m_DraggedItem->sp.setScale(0.8f, 0.8f);
		renderTarget->draw(m_DraggedItem->sp);
	}

	// Draw item preview
	drawItemPreview(renderTarget);
}

bool InventoryScreen::onEnter()
{
	m_SelectedSlot = 0;
	m_ScrollOffset = 0;
	m_SelectionAnimOffset = 0.0f;
	m_ItemAnimTimer = 0.0f;
	m_DraggedItem = nullptr;
	m_DraggedFromIndex = -1;
	return true;
}

int InventoryScreen::getSelectedItemIndex() const
{
	return m_ScrollOffset * GRID_WIDTH + m_SelectedSlot;
}

void InventoryScreen::drawGrid(RendTarget* renderTarget)
{
	auto plr = IGS->player;
	int gridStartX = GRID_START;
	int gridStartY = GRID_START;
	int totalSlots = GRID_WIDTH * GRID_HEIGHT;

	// Draw grid slots
	for(int i = 0; i < totalSlots; i++)
	{
		int x = i % GRID_WIDTH;
		int y = i / GRID_WIDTH;
		int slotX = gridStartX + x * (SLOT_SIZE + SLOT_SPACING);
		int slotY = gridStartY + y * (SLOT_SIZE + SLOT_SPACING);

		// Slot background
		sf::RectangleShape slot(sf::Vector2f(SLOT_SIZE, SLOT_SIZE));
		slot.setPosition(slotX, slotY);
		slot.setFillColor(sf::Color(60, 65, 75));
		slot.setOutlineColor(i == m_SelectedSlot ? sf::Color::Cyan : sf::Color(80, 85, 95));
		slot.setOutlineThickness(i == m_SelectedSlot ? 2 : 1);
		renderTarget->draw(slot);

		// Item in slot
		int itemIndex = m_ScrollOffset * GRID_WIDTH + i;
		if(itemIndex < (int)plr->inv->items.size())
		{
			Item* item = plr->inv->items[itemIndex];
			if(item->tex && item != m_DraggedItem)
			{
				float scale = 0.8f;
				if(i == m_SelectedSlot)
				{
					float pulse = std::sin(m_ItemAnimTimer * 3.0f) * 0.1f + 0.9f;
					scale *= pulse;
				}
				item->sp.setPosition(slotX + SLOT_SIZE/2 - 8, slotY + SLOT_SIZE/2 - 8);
				item->sp.setScale(scale, scale);
				renderTarget->draw(item->sp);
			}

			// Equipped indicator
			if(item == plr->inv->equippedItem && item != m_DraggedItem)
			{
				sf::CircleShape equippedDot(3);
				equippedDot.setPosition(slotX + SLOT_SIZE - 8, slotY + 2);
				equippedDot.setFillColor(sf::Color::Green);
				renderTarget->draw(equippedDot);
			}
		}
	}

	// Scroll indicator
	int totalRows = (plr->inv->items.size() + GRID_WIDTH - 1) / GRID_WIDTH;
	if(totalRows > GRID_HEIGHT)
	{
		sf::Text scrollText("Row " + std::to_string(m_ScrollOffset + 1) + "/" + std::to_string(totalRows - GRID_HEIGHT + 1), *Fonts::OSDFont, 14);
		scrollText.setFillColor(sf::Color(180, 180, 180));
		scrollText.setPosition(gridStartX + GRID_WIDTH * (SLOT_SIZE + SLOT_SPACING) + 10, gridStartY);
		renderTarget->draw(scrollText);
	}
}

void InventoryScreen::drawItemPreview(RendTarget* renderTarget) const
{
	auto plr = IGS->player;
	int itemIndex = getSelectedItemIndex();
	if(itemIndex >= (int)plr->inv->items.size()) return;

	Item* selectedItem = plr->inv->items[itemIndex];
	int previewY = Game::ScreenHeight - 150;

	// Preview panel
	sf::RectangleShape previewPanel(sf::Vector2f(Game::ScreenWidth - 140, 120));
	previewPanel.setPosition(70, previewY);
	previewPanel.setFillColor(sf::Color(50, 55, 65, 200));
	previewPanel.setOutlineColor(sf::Color::Cyan);
	previewPanel.setOutlineThickness(1);
	renderTarget->draw(previewPanel);

	// Large item sprite
	if(selectedItem->tex)
	{
		selectedItem->sp.setPosition(90, previewY + 20);
		selectedItem->sp.setScale(2.0f, 2.0f);
		renderTarget->draw(selectedItem->sp);
	}

	// Item name
	std::string text = selectedItem->name;
	if(selectedItem == plr->inv->equippedItem) text += " [EQUIPPED]";
	if(HotPocket* hp = dynamic_cast<HotPocket*>(selectedItem))
	{
		if(hp->cooked) text += " (Cooked)";
		if(hp->wrapped) text += " (Wrapped)";
	}
	if(Diaper* diaper = dynamic_cast<Diaper*>(selectedItem))
	{
		text += diaper->used ? " (Used)" : " (Clean)";
	}
	if(GasCan* gasCan = dynamic_cast<GasCan*>(selectedItem))
	{
		text += gasCan->filled ? " (Filled)" : " (Empty)";
	}

	sf::Text itemText(text, *Fonts::OSDFont, 20);
	itemText.setFillColor(sf::Color::White);
	if(selectedItem == plr->inv->equippedItem)
	{
		itemText.setFillColor(sf::Color(100, 255, 100));
		itemText.setOutlineColor(sf::Color(50, 150, 50));
		itemText.setOutlineThickness(1);
	}
	itemText.setPosition(150, previewY + 20);
	renderTarget->draw(itemText);

	// Item description
	if(!selectedItem->description.empty())
	{
		sf::Text descText(selectedItem->description, *Fonts::OSDFont, 16);
		descText.setFillColor(sf::Color(200, 200, 200));
		descText.setPosition(150, previewY + 50);
		renderTarget->draw(descText);
	}
}

void InventoryScreen::handleGridInput()
{
	auto plr = IGS->player;
	if(plr->inv->items.empty()) return;

	int oldSlot = m_SelectedSlot;
	int oldScroll = m_ScrollOffset;

	// Grid navigation
	if(Kb::IsKeyReleased(KB::Left) || Kb::IsKeyReleased(KB::A))
	{
		m_SelectedSlot--;
		if(m_SelectedSlot < 0) m_SelectedSlot = GRID_WIDTH - 1;
	}
	if(Kb::IsKeyReleased(KB::Right) || Kb::IsKeyReleased(KB::D))
	{
		m_SelectedSlot++;
		if(m_SelectedSlot >= GRID_WIDTH) m_SelectedSlot = 0;
	}
	if(Kb::IsKeyReleased(KB::Up) || Kb::IsKeyReleased(KB::W))
	{
		m_SelectedSlot -= GRID_WIDTH;
		if(m_SelectedSlot < 0)
		{
			if(m_ScrollOffset > 0)
			{
				m_ScrollOffset--;
				m_SelectedSlot += GRID_WIDTH;
			}
			else
			{
				m_SelectedSlot += GRID_WIDTH;
			}
		}
	}
	if(Kb::IsKeyReleased(KB::Down) || Kb::IsKeyReleased(KB::S))
	{
		m_SelectedSlot += GRID_WIDTH;
		if(m_SelectedSlot >= GRID_WIDTH * GRID_HEIGHT)
		{
			int totalRows = (plr->inv->items.size() + GRID_WIDTH - 1) / GRID_WIDTH;
			if(m_ScrollOffset < totalRows - GRID_HEIGHT)
			{
				m_ScrollOffset++;
				m_SelectedSlot -= GRID_WIDTH;
			}
			else
			{
				m_SelectedSlot -= GRID_WIDTH;
			}
		}
	}

	// Clamp selection to valid items
	int itemIndex = getSelectedItemIndex();
	if(itemIndex >= (int)plr->inv->items.size())
	{
		m_SelectedSlot = oldSlot;
		m_ScrollOffset = oldScroll;
	}

	// Actions
	if(Kb::IsKeyReleased(KB::Enter) || Kb::IsKeyReleased(KB::E))
	{
		int idx = getSelectedItemIndex();
		if(idx < (int)plr->inv->items.size())
		{
			plr->inv->equippedItem = plr->inv->items[idx];
		}
	}

	if(Kb::IsKeyReleased(KB::X))
	{
		int idx = getSelectedItemIndex();
		if(idx < (int)plr->inv->items.size())
		{
			Item* itemToDrop = plr->inv->items[idx];

			// Remove from inventory
			if(itemToDrop == plr->inv->equippedItem)
			{
				plr->inv->equippedItem = nullptr;
			}
			plr->inv->items.erase(plr->inv->items.begin() + idx);

			// Calculate position in front of player
			double dropPosX = plr->posX;
			double dropPosY = plr->posY;
			if(plr->getDir() == Direction_Up) dropPosY -= 20;
			else if(plr->getDir() == Direction_Down) dropPosY += 20;
			else if(plr->getDir() == Direction_Left) dropPosX -= 20;
			else if(plr->getDir() == Direction_Right) dropPosX += 20;

			// Set item position and add to map
			itemToDrop->posX = dropPosX;
			itemToDrop->posY = dropPosY;
			itemToDrop->updateTPos();
			itemToDrop->inv = nullptr;

			plr->hostMap->addEnt(itemToDrop);

			// Adjust selection if needed
			if(getSelectedItemIndex() >= (int)plr->inv->items.size() && !plr->inv->items.empty())
			{
				if(m_SelectedSlot > 0) m_SelectedSlot--;
				else if(m_ScrollOffset > 0) m_ScrollOffset--;
			}
		}
	}
}

void InventoryScreen::handleMouseInput()
{
	auto plr = IGS->player;
	
	// Left click to select or start drag
	if(Mouse::LeftFrames == 1 && !m_DraggedItem)
	{
		int slot = getSlotAtPosition(Mouse::Pos_X, Mouse::Pos_Y);
		if(slot >= 0)
		{
			int itemIndex = m_ScrollOffset * GRID_WIDTH + slot;
			if(itemIndex < (int)plr->inv->items.size())
			{
				m_SelectedSlot = slot;
				m_DraggedItem = plr->inv->items[itemIndex];
				m_DraggedFromIndex = itemIndex;
				int gridStartX = GRID_START;
				int gridStartY = GRID_START;
				int x = slot % GRID_WIDTH;
				int y = slot / GRID_WIDTH;
				int slotX = gridStartX + x * (SLOT_SIZE + SLOT_SPACING);
				int slotY = gridStartY + y * (SLOT_SIZE + SLOT_SPACING);
				m_DragOffsetX = slotX - Mouse::Pos_X;
				m_DragOffsetY = slotY - Mouse::Pos_Y;
			}
		}
	}
	
	// End drag
	if(Mouse::LeftRel && m_DraggedItem)
	{
		int targetSlot = getSlotAtPosition(Mouse::Pos_X, Mouse::Pos_Y);
		if(targetSlot >= 0)
		{
			int targetIndex = m_ScrollOffset * GRID_WIDTH + targetSlot;
			if(targetIndex != m_DraggedFromIndex)
			{
				// Remove from original position
				plr->inv->items.erase(plr->inv->items.begin() + m_DraggedFromIndex);
				
				// Insert at new position
				if(targetIndex >= (int)plr->inv->items.size())
				{
					plr->inv->items.push_back(m_DraggedItem);
				}
				else
				{
					if(targetIndex > m_DraggedFromIndex) targetIndex--;
					plr->inv->items.insert(plr->inv->items.begin() + targetIndex, m_DraggedItem);
				}
			}
		}
		m_DraggedItem = nullptr;
		m_DraggedFromIndex = -1;
	}
}

int InventoryScreen::getSlotAtPosition(int posX, int posY) const
{
	int gridStartX = GRID_START;
	int gridStartY = GRID_START;
	
	if(posX < gridStartX || posY < gridStartY) return -1;
	
	int x = (posX - gridStartX) / (SLOT_SIZE + SLOT_SPACING);
	int y = (posY - gridStartY) / (SLOT_SIZE + SLOT_SPACING);
	
	if(x >= GRID_WIDTH || y >= GRID_HEIGHT) return -1;
	if((posX - gridStartX) % (SLOT_SIZE + SLOT_SPACING) >= SLOT_SIZE) return -1;
	if((posY - gridStartY) % (SLOT_SIZE + SLOT_SPACING) >= SLOT_SIZE) return -1;
	
	return y * GRID_WIDTH + x;
}

void InventoryScreen::switchTo(bool noFadeTamper)
{
	Sfx::Close->play();
	Screen::switchTo(noFadeTamper);	
}

}
