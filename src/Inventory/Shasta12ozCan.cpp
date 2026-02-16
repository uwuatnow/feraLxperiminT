#include "Inventory/Shasta12ozCan.h"
#include <iostream>

namespace nyaa {

Shasta12ozCan::Shasta12ozCan()
	:Item("Diet Shasta Cola (12oz can)", "I love sucralose! (may cause diabetes)")
	,cracked(false)
	,empty(false)
{

}

Shasta12ozCan::~Shasta12ozCan()
{
	//std::cout << "shasta dtor" << std::endl;
}

}