#pragma once
#include "Game/Globals.h"
#include "Screen/Screen.h"
#include "Text/TextInput.h"
#include "Game/SelectionPrompt.h"

namespace nyaa {

class EditTextScreen : public Screen
{
public:
	static EditTextScreen* Instance;

	EditTextScreen();

	virtual ~EditTextScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;

	virtual bool onEnter() override;

	virtual bool onLeave() override;

public:
	std::string* str;
	TextInput ti;
	SelectionPrompt* sp;
};

}