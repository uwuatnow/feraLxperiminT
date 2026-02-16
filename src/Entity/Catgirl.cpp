#include "Entity/Catgirl.h"

namespace nyaa {

Catgirl::Catgirl()
	:Actor(
		"neko", 
		255, 255, 255, 255, 
		255, 0, 255, 255, 
		0, 255, 255, 255, 
		255, 255, 255, 255, 
		0, 255, 255, 255, 
		255, 255, 255, 255
	)
{
	
}

Catgirl::Catgirl(
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
	)
	:Actor(
		"neko", 
		bodyColR, bodyColG, bodyColB, bodyColA,
		eyeColR, eyeColG, eyeColB, eyeColA,
		hairColR, hairColG, hairColB, hairColA,
		clothesColR, clothesColG, clothesColB, clothesColA,
		tailColR, tailColG, tailColB, tailColA,
		outlineColR, outlineColG, outlineColB, outlineColA
	)
{
	
}

} // namespace nyaa