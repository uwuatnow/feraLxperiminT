#include "Entity/Explosion.h"

namespace nyaa {

Explosion::Explosion()
	:Prop("explosion", 4, 5)
{
	originX = 0.5f;
	originY = 1.0f;
}

Explosion::~Explosion()
{
}

} // namespace nyaa