#pragma once
#include "Screen/Screen.h"
#include <string>
#include <vector>
#include <SFML/System/Clock.hpp>
#include <random>

namespace nyaa {

class Computer;

class PCScreen : public Screen
{
public:
	static PCScreen* Instance;

	PCScreen();

	virtual ~PCScreen();

public:
	virtual void doTick(RendTarget* renderTarget) override;

	virtual bool onEnter() override;

	virtual bool onLeave() override;

private:
	void handleDesktopInput();
	void renderDesktop(RendTarget* renderTarget);

public:
	Computer* accessedFrom; // The computer prop that triggered this screen

	//dont put anything here, all state goes inside accessedFrom
};

}