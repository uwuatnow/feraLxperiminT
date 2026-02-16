#pragma once
#include "Game/Globals.h"
#include "Entity/Entity.h"

namespace nyaa {

class BulletProjectile : public Entity
{
public:
	BulletProjectile(double posX, double posY, float dirAngle, int damage);
	virtual ~BulletProjectile();

public:
	virtual void update() override;
	virtual class OBB* getOBB() const override;

public:
	static float BulletRadius; // radius of the bullet for collision detection

public:
	unsigned int collideLimit;
	bool ricochet;
	float speed;
	Entity* shooter;
	class Timer* deleteT;
	int damage;
};

} // namespace nyaa