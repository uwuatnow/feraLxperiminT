#include "Entity/Light.h"
#include "Game/Util.h"
#include "Map/Map.h"

namespace nyaa {

Light::Light()
    :Prop("light", 1, 1, Direction_Center, true)
    ,colorR(255)
    ,colorG(255)
    ,colorB(255)
    ,intensity(1.0f)
    ,lightId(-1)
{
    originX = 0.5f;
    originY = 0.5f;
    friction = 0.06f;
    flags |= EntFlag_UnderAllOtherEnts;
}

Light::~Light()
{
	//Util::PrintLnFormat("Light at %.2fx %.2fy in map %s deleted\n", posX, posY, hostMap->name.c_str());
}

void Light::update()
{
    Prop::update();
}

} // namespace nyaa