#include "Map/RainDrop.h"
#include "Game/Game.h"
#include "Game/Util.h"

namespace nyaa {

RainDrop::RainDrop()
	:x((Game::ScreenWidth / 2) + (Util::NormalizedDistro(Util::mtGEN) * (Game::ScreenWidth / 2)))
	,y(0)
	,dirHoriz(Util::NormalizedDistro(Util::mtGEN))
	,frame(0)
{

}

}