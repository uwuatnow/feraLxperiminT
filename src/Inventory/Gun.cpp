#include "Inventory/Gun.h"
#include "Game/Util.h"
#include "Entity/Actor.h"
#include "Screen/InGameScreen.h"
#include "Entity/BulletProjectile.h"
#include "Game/Game.h"
#include "Mission/Mission.h"
#include "Game/Sfx.h"
#include "Game/Mouse.h"
#include "Inventory/Inventory.h"

namespace nyaa {

Gun::Gun(const std::string& name, const std::string& desc, const std::string& texName, GunType type)
	:Item(name, desc, texName, 1, 1, 0)
	,gunType(type)
	,rattleCooldown(300)
	,gunAngle(0)
{
	invSpriteOffsetX = -5;
	invSpriteOffsetY = 0;
	sizeX = 16;
	sizeY = 3;
	renderScaleX = 3;
	renderScaleY = 10;
	mass = 50;
	friction = 20.f;
	
	// Compute inertia from mass and size for realistic rotation physics
	inertiaScale = 0.5f; // Guns spin a bit easier than their mass would suggest
	computeInertia();
}

Gun* Gun::Create(GunType type)
{
	switch (type) {
	case GunType::Pistol:
		return new Pistol();
	case GunType::Rifle:
		return new Rifle();
	default:
		return new Pistol();
	}
}

Pistol::Pistol()
	:Gun("Glocc", "switch installed", "gun", GunType::Pistol)
{
	sizeX = 16;
	sizeY = 1.5;
	renderScaleX = 1.5;
	renderScaleY = 15;
	friction = 0.9;
	mass = 2.0f; // Pistols are light
	computeInertia();
}

Rifle::Rifle()
	:Gun("Ruffle", "a powerful one", "gun", GunType::Rifle)
{
	sizeX = 16;
	sizeY = 3;
	renderScaleX = 3;
	renderScaleY = 10;
	friction = 1.9;
	mass = 4.5f; // Rifles are heavier
	computeInertia();
}

void Gun::use()
{
	shootBullet();
}

void Gun::update()
{
	Entity::update();
	rattleCooldown.update();
}

void Gun::shootBullet()
{
	Actor * a = inv->owner;
#if DEBUG
	if (IGS->tileEditOn || !IGS->movingV2fps.empty() || IGS->grabbedCollAnchor) return;
#endif
	if (!invervalTimer.once(getFireRate())) return;
	
	// Use the current actor direction as the base for the bullet angle
	float bulletAngle = fmodf(a->dirAngle, 360);

	double bulletX, bulletY;
	a->getHandPos(bulletX, bulletY);
	sf::Vector2f bulletOrigin((float)bulletX, (float)bulletY);
	bulletOrigin.x -= 2;

	// Aim Assist for controller
	if (a == IGS->player && G->inMethod == InputMethod_Controller)
	{
		Actor* bestTarget = nullptr;
		double bestDiff = 18.0; // 10% tolerance (36 total, +/- 18)

		for (Entity* e : a->hostMap->getEntities())
		{
			Actor* targetActor = dynamic_cast<Actor*>(e);
			if (targetActor && targetActor != a && !(targetActor->flags & EntFlag_Dead))
			{
				// Aim from the actual bullet origin to the target's center (posY - 4.0)
				double angleToTarget = (float)Util::RotateTowards(bulletOrigin.x, bulletOrigin.y, targetActor->posX, targetActor->posY - 4.0);
				double diff = std::abs(Util::AngleDifference(bulletAngle, angleToTarget));
				if (diff < bestDiff)
				{
					bestDiff = diff;
					bestTarget = targetActor;
				}
			}
		}

		if (bestTarget)
		{
			bulletAngle = (float)Util::RotateTowards(bulletOrigin.x, bulletOrigin.y, bestTarget->posX, bestTarget->posY - 4.0);
			// Update the actor's direction so the gun visually snaps to the target
			a->dirAngle = bulletAngle;
		}
	}

	// Calculate the spawn position offset based on the actual firing angle
	double frontPosX = 0, frontPosY = 0;
	Util::AngleLineRel(bulletOrigin.x, bulletOrigin.y, bulletAngle, 3, &frontPosX, &frontPosY);
	
	// Check for collision with the player before spawning
	if (!a->hostMap->doesLineIntersectCollisionLine(frontPosX, frontPosY, a->posX, a->posY))
	{
		BulletProjectile* bp = new BulletProjectile(frontPosX, frontPosY, bulletAngle, getDamage());
		bp->posZ = a->posZ + a->sizeZ * 0.9;
		bp->shooter = a;
		a->hostMap->addEnt(bp);
		Sfx::Shoot->play(a->posX, a->posY, a->posZ + a->sizeZ * 0.9); // Pass height at ear level
		recoilT.reset();
	}
}

void Gun::equippedUpdate()
{
	// For controller, the actor's direction (from stick input or aim assist) is the source of truth
	if (inv->owner == IGS->player && G->inMethod == InputMethod_Controller)
	{
		gunAngle = inv->owner->dirAngle;
	}
	else
	{
		inv->owner->dirAngle = gunAngle;
	}

	invervalTimer.update();
	recoilT.update();
	if (recoilT.secs() > 0.1) recoilT.setMillis(100);
}

void Gun::playCollisionSfx()
{
	// 300 ms cooldown per gun entity before it can rattle again
	if (rattleCooldown.once(300.0f))
	{
		Sfx::PlayRandomGunRattle((float)posX, (float)posY);
	}
}

}