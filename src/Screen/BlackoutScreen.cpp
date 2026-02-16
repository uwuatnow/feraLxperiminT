#include "Screen/BlackoutScreen.h"

namespace nyaa {

BlackoutScreen* BlackoutScreen::Instance = nullptr;

BlackoutScreen::BlackoutScreen()
{
	Instance = this;
}

BlackoutScreen::~BlackoutScreen()
{
}

bool BlackoutScreen::onEnter()
{
	/*if (callback.get()->valid()) {
		auto result = (*callback)();
		if (!result.valid()) {
			sol::error err = result;
			std::cerr << "Blackout Error: " << err.what() << '\n';
		}
	}*/
	return false;
}

}