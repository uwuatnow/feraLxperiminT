#include "Entity/BulletProjectile.h"
#include "Game/Util.h"
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include "Game/Game.h"
#include "Map/Map.h"
#include "Game/SaveData.h"
#include <SFML/Graphics/CircleShape.hpp>
#include "Game/Sfx.h"
#include "Game/Timer.h"
#include "Game/OBB.h"

namespace nyaa {

float BulletProjectile::BulletRadius = 0.5f;

BulletProjectile::BulletProjectile(double posX, double posY, float dirAngle, int damage)
	:collideLimit(3) //can hit 3 walls before deleting itself
	,speed(3.0f)
	,shooter(nullptr)
	,deleteT(new Timer())
	,damage(damage)
{
	this->posX = posX;
	this->posY = posY;
	this->dirAngle = dirAngle;
	flags |= EntFlag_Animating;
	ricochet = (double)Util::RandNormalized() >= 0.9;
}

BulletProjectile::~BulletProjectile()
{
	delete deleteT;
}

void BulletProjectile::update()
{
	portalActivateCooldown->update();
	deleteT->update();
	if (deleteT->secs() > 10 && !IGS->isPointInView(posX, posY))
	{
		flags |= EntFlag_Dead;
		return;
	}

	double mo = (double)speed * ((double)G->frameDeltaMillis / (1000.0 / (double)Game::FPSConstant)) * 2.55;
	
	// Calculate predicted movement
	double dx, dy;
	Util::AngleLineAbs(0, 0, (double)dirAngle, mo, &dx, &dy);
	double nextX = posX + dx;
	double nextY = posY + dy;

	// 1. Check for hits on ALL entities using OBB-Segment intersection
	Entity* hitEnt = nullptr;
	for (auto e : hostMap->entities) 
	{
		if (!e || e == this || (e->flags & EntFlag_Dead) || (e->flags & EntFlag_UnderAllOtherEnts) || e == shooter) continue;
		if (dynamic_cast<BulletProjectile*>(e)) continue;

		OBB* targetOBB = e->getOBB();
		// Accurate segment-vs-OBB collision
		if (OBBIntersectsLine(*targetOBB, (float)posX, (float)posY, (float)nextX, (float)nextY, nullptr))
		{
			hitEnt = e;
			break;
		}
	}

	if (hitEnt)
	{
		// Process entity hit
		hitEnt->hurt((float)damage, DamageReason_Violence, shooter);
		
		// If it's an actor, turn it towards the shooter
		if (auto actor = dynamic_cast<Actor*>(hitEnt)) {
			actor->turnTowards(this);
			Sfx::Hurt->play((float)hitEnt->posX, (float)hitEnt->posY);
		}

		// Apply physical impact
		float rx, ry;
		hitEnt->physicsPush(posX, posY, 1.0f, &rx, &ry);
		
		// Move bullet to approximate hit location for visual consistency
		posX = (posX + hitEnt->posX) * 0.5;
		posY = (posY + hitEnt->posY) * 0.5;
		
		flags |= EntFlag_Dead;
		Sfx::WallHit->play((float)posX, (float)posY);
		return;
	}

	// 2. Check for wall collisions
	auto collidedLine = checkCollide(hostMap->collLines, (float)mo, dirAngle, (float)mo + BulletProjectile::BulletRadius, nullptr);

	if (collidedLine != nullptr) 
	{
		Sfx::WallHit->play((float)posX, (float)posY);
		if (ricochet) {
			--collideLimit;
			if (collideLimit == 0) 
			{
				flags |= EntFlag_Dead;
			}
			else 
			{
				speed *= 0.9f;
				// Reflect the bullet off the wall
				double newAngle = Util::ReflectFromLine(
					(double)collidedLine->p1.x, (double)collidedLine->p1.y,
					(double)collidedLine->p2.x, (double)collidedLine->p2.y,
					posX, posY, (double)dirAngle);
				double r = (double)Util::RandNormalized();
				dirAngle = (float)(newAngle + (r - 0.5) * 60.0);
			}
		}
		else 
		{
			flags |= EntFlag_Dead;
		}
	}
	else 
	{
		// No collisions: commit movement
		lposX = posX;
		lposY = posY;
		posX = nextX;
		posY = nextY;
	}
}

OBB* BulletProjectile::getOBB() const
{
	m_OBB->center = { posX, posY };
	m_OBB->halfExtents = { BulletRadius, BulletRadius };
	m_OBB->angle = dirAngle;
	return m_OBB;
}

} // namespace nyaa