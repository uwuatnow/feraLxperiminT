#pragma once
#include "Entity/Catgirl.h"

namespace nyaa {

class DealerCatgirl : public Catgirl
{
public:
	DealerCatgirl(
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

    unsigned int botherCount;

    void update() override;

	void hurt(float amount, DamageReason damageReason, Entity* damageSource = nullptr) override;
};

} // namespace nyaa