#pragma once
#include "Game/Globals.h"
#include "Entity/Actor.h"

namespace nyaa {

class Catgirl : public Actor
{
public:
	Catgirl(); /* with default colors */

	Catgirl(
		unsigned char bodyColR,
		unsigned char bodyColG,
		unsigned char bodyColB,
		unsigned char bodyColA,

		unsigned char eyeColR,
		unsigned char eyeColG,
		unsigned char eyeColB,
		unsigned char eyeColA,

		unsigned char hairColR,
		unsigned char hairColG,
		unsigned char hairColB,
		unsigned char hairColA,

		unsigned char clothesColR,
		unsigned char clothesColG,
		unsigned char clothesColB,
		unsigned char clothesColA,

		unsigned char tailColR,
		unsigned char tailColG,
		unsigned char tailColB,
		unsigned char tailColA,

		unsigned char outlineColR,
		unsigned char outlineColG,
		unsigned char outlineColB,
		unsigned char outlineColA
	);
};

} // namespace nyaa