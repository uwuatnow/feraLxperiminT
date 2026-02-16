#pragma once
#include "Screen/Screen.h"
#include "Game/SelectionPrompt.h"

namespace nyaa {

class Portal;

class PortalEditScreen : public Screen
{
public:
	static PortalEditScreen* Instance;

	PortalEditScreen();
	
	virtual ~PortalEditScreen();
	
public:
	virtual void doTick(RendTarget* renderTarget) override;

	virtual bool onEnter() override;
	
public:
	SelectionPrompt sp;
	Portal * editingPortal;
	SelectionPrompt::Selection & ofdSel, & owdSel;
	SelectionPrompt::Selection* tileXSel;
	SelectionPrompt::Selection* tileYSel;
};

}