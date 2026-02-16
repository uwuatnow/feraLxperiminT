#pragma once
#include "Game/Globals.h"

#include <list>
#include <string>
#include <functional>
#include <SFML/Graphics/Text.hpp>
#include "Game/Timer.h"
#include "Direction.h"
#include "Game/GuiWidget.h"

namespace nyaa {

enum SelPResp
{
	SelPResp_Neutral,
	SelPResp_DidSomething
};

class SelectionPrompt : public GuiWidget
{
public:
	class Selection;

	typedef std::function<SelPResp(SelectionPrompt& sp, Selection& s, Direction d)> SPCallback;

	class Selection : public GuiWidget
	{
	public:
		Selection(SelectionPrompt& host, std::string ttext, bool enabled);
		void setValue(std::string val, 
			unsigned char fillColR = 255, 
			unsigned char fillColG = 0, 
			unsigned char fillColB = 0, 

			unsigned char outlineColR = 0,
			unsigned char outlineColG = 255,
			unsigned char outlineColB = 0
		);
		void setText(std::string text);
		void update(bool isSelected);
		
	public:
		SelectionPrompt& host;
		SPCallback callback;
		sf::Text text;
		sf::Text valText;
		Timer animTimer;
		bool animUp;
		unsigned int animMs;
		bool enabled;
		bool isClicked;
		Selection *bef, *next;
		unsigned char fillColorR, fillColorG, fillColorB;
		unsigned char outlineColorR, outlineColorG, outlineColorB;
		unsigned char valFillColorR, valFillColorG, valFillColorB;
		unsigned char valOutlineColorR, valOutlineColorG, valOutlineColorB;
		Direction mouseScroll;
		int index;
	};

public:
	SelectionPrompt(float rectLeft, float rectTop, float rectWidth, float rectHeight, bool vertical = true);
	
	~SelectionPrompt();
	
public:
	Selection& add(std::string text, SPCallback cb, bool enabled = true);
	
	//int getClicked(); /* returns -1 if nothing was selected */
	
	void update(bool focus = true);
	
	void render(RendTarget* renderTarget);
	
	Selection* getSelected();
	
	void updateScrollOffset();
	
	// Hierarchical navigation support
	void clearSelections();
	void setBreadcrumb(std::string breadcrumb);
	std::string getBreadcrumb() const;

public:
	float rectLeft, rectTop, rectWidth, rectHeight;
	unsigned int selectedIndex;
	bool vertical;
	bool upFail;
	std::list<Selection> selections;
	float scrollOffset;
	Clock scrollTimer;
	float scrollStartY;
	float scrollTargetY;
	
	// Hierarchical navigation
	std::string breadcrumb;
	bool showBreadcrumb;
};

}