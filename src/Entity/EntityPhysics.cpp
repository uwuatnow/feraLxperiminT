#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Game/Util.h"
#include "Game/Sfx.h"
#include <SFML/Graphics.hpp>
#include "Map/Map.h"
#include "Map/CollisionLine.h"
#include "Entity/BulletProjectile.h"
#include "Entity/CarBase.h"
#include "Entity/Actor.h"
#include "Entity/BuildingWallEntity.h"
#include "Game/OBB.h"

namespace nyaa {

void Entity::getHitBox(double* outLeft, double* outTop, float* outWidth, float* outHeight)
{
	if (sizeX > 0 && sizeY > 0)
	{
		if(outLeft) *outLeft = posX - (double)originX * (double)sizeX;
		if(outTop) *outTop = posY - (double)originY * (double)sizeY;
		if(outWidth) *outWidth = sizeX;
		if(outHeight) *outHeight = sizeY;
	}
	else
	{
		if(outLeft) *outLeft = posX - 3.0;
		if(outTop) *outTop = posY - 10.0;
		if(outWidth) *outWidth = 6.0f;
		if(outHeight) *outHeight = 8.0f;
	}
}

OBB* Entity::getOBB() const
{
	if (sizeX > 0 && sizeY > 0)
	{
		// Adjusted center based on origin
		double cx = posX + (0.5 - (double)originX) * (double)sizeX;
		double cy = posY + (0.5 - (double)originY) * (double)sizeY;
		m_OBB->center = { cx, cy };
		m_OBB->halfExtents = { (double)sizeX * 0.5, (double)sizeY * 0.5 };
		m_OBB->angle = dirAngle;
	}
	else
	{
		// Match Touhou hitbox fallback
		m_OBB->center = { posX, posY - 6.0 }; // Roughly center of 6x8 box shifted by -10 top
		m_OBB->halfExtents = { 3.0, 4.0 };
		m_OBB->angle = 0; // Actors don't rotate their hitbox usually
	}
	return m_OBB;
}

void Entity::resolveCollisionWith(Entity* e)
{
	if ((flags & EntFlag_Dead) || !hostMap || !e || e == this || (e->flags & EntFlag_Dead)) return;
	if ((flags & EntFlag_UnderAllOtherEnts) || (e->flags & EntFlag_UnderAllOtherEnts)) return;
	if (dynamic_cast<BulletProjectile*>(this) || dynamic_cast<BulletProjectile*>(e)) return;
	
    // Skip collision between cars and building walls - handled separately in CarBase::update()
    if ((dynamic_cast<CarBase*>(this) && dynamic_cast<BuildingWallEntity*>(e)) ||
        (dynamic_cast<BuildingWallEntity*>(this) && dynamic_cast<CarBase*>(e))) {
        return;
    }

    // Skip collision between building walls - walls should not affect each other's physics
    if (dynamic_cast<BuildingWallEntity*>(this) && dynamic_cast<BuildingWallEntity*>(e)) {
        return;
    }

    OBB* thisOBB = getOBB();
	OBB* otherOBB = e->getOBB();
	Point mtv;
	
	if (OBBIntersects(*thisOBB, *otherOBB, &mtv))
	{
		// Special handling for actors hitting building walls
		if (auto actor = dynamic_cast<Actor*>(this)) {
			if (auto wall = dynamic_cast<BuildingWallEntity*>(e)) {
				// Actor is hitting a wall - treat wall as immovable during collision
				// Only move the actor, not the wall
				// Actors cannot damage or push building walls - only cars can
				posX += mtv.x;
				posY += mtv.y;
				return;
			}
		}
		// Same check but reversed (wall is 'this', actor is 'e')
		if (auto wall = dynamic_cast<BuildingWallEntity*>(this)) {
			if (auto actor = dynamic_cast<Actor*>(e)) {
				// Actor is hitting a wall - treat wall as immovable during collision
				// Only move the actor, not the wall
				// Actors cannot damage or push building walls - only cars can
				e->posX -= mtv.x;
				e->posY -= mtv.y;
				return;
			}
		}

		// Check z difference - if too high, no collision (allows jumping over)
		float zDiff = std::abs(posZ - e->posZ);
		if (zDiff > 16.0f) return;

		// Collision normal is mtv normalized
		double mtvLen = std::sqrt(mtv.x * mtv.x + mtv.y * mtv.y);
		if (mtvLen < 0.0001) return;
		Point normal;
		normal.x = mtv.x / mtvLen;
		normal.y = mtv.y / mtvLen;

		// Get entity centers
		auto getCenter = [](Entity* ent) -> Point {
			return { ent->posX + (0.5 - (double)ent->originX) * (double)ent->sizeX, ent->posY + (0.5 - (double)ent->originY) * (double)ent->sizeY };
		};

		Point c1 = getCenter(this);
		Point c2 = getCenter(e);
		
		// Find actual contact point using OBB closest point calculation
		// The contact point is where the entities actually touch, not the midpoint of centers
		Point closestOnThis = OBBClosestPoint(*thisOBB, otherOBB->center);
		Point closestOnOther = OBBClosestPoint(*otherOBB, thisOBB->center);
		Point contactPoint;
		contactPoint.x = (closestOnThis.x + closestOnOther.x) * 0.5;
		contactPoint.y = (closestOnThis.y + closestOnOther.y) * 0.5;

		// Calculate offset from each entity's center to the contact point
		Point r1 = { contactPoint.x - c1.x, contactPoint.y - c1.y };
		Point r2 = { contactPoint.x - c2.x, contactPoint.y - c2.y };

		// Cross products for torque calculation (r × normal in 2D gives scalar)
		float r1CrossN = (float)(r1.x * normal.y - r1.y * normal.x);
		float r2CrossN = (float)(r2.x * normal.y - r2.y * normal.x);

		// Calculate effective masses including rotational contribution
		float invM1 = 1.0f / mass;
		float invM2 = 1.0f / e->mass;
		float invI1 = 1.0f / inertia;
		float invI2 = 1.0f / e->inertia;

		// Effective mass at contact point includes both linear and angular components
		float effectiveInvMass1 = invM1 + r1CrossN * r1CrossN * invI1;
		float effectiveInvMass2 = invM2 + r2CrossN * r2CrossN * invI2;
		float totalEffectiveInvMass = effectiveInvMass1 + effectiveInvMass2;

		// Distribute the MTV correction based on effective mass (includes rotational inertia)
		float myMoveRatio = effectiveInvMass1 / totalEffectiveInvMass;
		float otherMoveRatio = effectiveInvMass2 / totalEffectiveInvMass;

		// Apply positional correction
		posX += mtv.x * myMoveRatio;
		posY += mtv.y * myMoveRatio;
		e->posX -= mtv.x * otherMoveRatio;
		e->posY -= mtv.y * otherMoveRatio;

		// Apply rotational correction - torque from off-center push
		// The push creates a torque = r × F, which causes angular displacement
		float correctionMag = (float)mtvLen;
		float angularCorrection1 = Util::ToDeg(r1CrossN * correctionMag * invI1 / totalEffectiveInvMass);
		float angularCorrection2 = Util::ToDeg(r2CrossN * correctionMag * invI2 / totalEffectiveInvMass);
		
		// Scale factor for gameplay tuning
		float rotationScale = 2.0f;
		dirAngle += angularCorrection1 * rotationScale;
		e->dirAngle -= angularCorrection2 * rotationScale;
		
		auto getPointVel = [&](Entity* ent, Point contact) -> Point {
			float vx, vy;
			ent->getVel(&vx, &vy);
			if (auto c = dynamic_cast<CarBase*>(ent)) {
				vx = c->velX;
				vy = c->velY;
			}
			
			// Rotational velocity: v = w x r
			Point r;
			r.x = contact.x - getCenter(ent).x;
			r.y = contact.y - getCenter(ent).y;
			float angVelRad = Util::ToRad(ent->angularVelocity);
			return { (double)vx + (double)(-angVelRad * (float)r.y), (double)vy + (double)(angVelRad * (float)r.x) };
		};

		Point v1 = getPointVel(this, contactPoint);
		Point v2 = getPointVel(e, contactPoint);
		Point rv;
		rv.x = v1.x - v2.x;
		rv.y = v1.y - v2.y;

		double vDotN = rv.x * normal.x + rv.y * normal.y;
		
		// If moving towards each other
		if (vDotN < 0) {
			float restitution = 0.3f;
			float j = -(1.0f + restitution) * vDotN;

			// Torque terms
			Point r1;
			r1.x = contactPoint.x - c1.x;
			r1.y = contactPoint.y - c1.y;
			Point r2;
			r2.x = contactPoint.x - c2.x;
			r2.y = contactPoint.y - c2.y;
			float r1CrossN = r1.x * normal.y - r1.y * normal.x;
			float r2CrossN = r2.x * normal.y - r2.y * normal.x;

			float invM1 = 1.0f / mass;
			float invM2 = 1.0f / e->mass;
			float invI1 = 1.0f / inertia;
			float invI2 = 1.0f / e->inertia;

			float impulse = j / (invM1 + invM2 + (r1CrossN * r1CrossN * invI1) + (r2CrossN * r2CrossN * invI2));
			Point impulseVec;
			impulseVec.x = normal.x * impulse;
			impulseVec.y = normal.y * impulse;

			// Apply to this
			if (auto car1 = dynamic_cast<CarBase*>(this)) {
				car1->velX += impulseVec.x * invM1;
				car1->velY += impulseVec.y * invM1;
			} else {
				physicsSlideAngle = Util::RepairAngle(Util::ToDeg(std::atan2(-impulseVec.y, -impulseVec.x)));
				physicsSlideAmount += impulse * invM1;
			}
			angularVelocity += Util::ToDeg(r1CrossN * impulse * invI1);

			// Apply to other
			if (auto car2 = dynamic_cast<CarBase*>(e)) {
				car2->velX -= impulseVec.x * invM2;
				car2->velY -= impulseVec.y * invM2;
			} else {
				e->physicsSlideAngle = Util::RepairAngle(Util::ToDeg(std::atan2(impulseVec.y, impulseVec.x)));
				e->physicsSlideAmount += impulse * invM2;
			}
			e->angularVelocity -= Util::ToDeg(r2CrossN * impulse * invI2);
			
			e->playCollisionSfx();

			// Damage occupants if they are cars
			auto damageOccupant = [&](CarBase* car, Entity* other, float impuls) {
				if (car->occupant) {
					if (auto actor = dynamic_cast<Actor*>(car->occupant)) {
						// Damage is now proportional to the change in velocity (deltaV)
						// which naturally accounts for the mass of the other object.
						float deltaV = impuls / car->mass;
						float damage = deltaV * 0.06f; 
						if (damage > 0.5f) {
							actor->hurt(damage, Actor::DamageReason_VehicleCollision, other);
						}
					}
				}
			};

			if (auto car1 = dynamic_cast<CarBase*>(this)) {
				damageOccupant(car1, e, impulse);
			}
			if (auto car2 = dynamic_cast<CarBase*>(e)) {
				damageOccupant(car2, this, impulse);
			}
		}
	}
}

void Entity::physicsPush(double fromX, double fromY, float amount, float* outX, float* outY)
{
    if (flags & EntFlag_UnderAllOtherEnts)
    {
        return/* sf::Vector2f(0, 0)*/;
    }
    //amount *= friction;
    double dt = (double)G->frameDeltaMillis / 1000.0;
    double moveDist = (double)amount * dt;
	Point diff = {};
	double rott = Util::RotateTowards(fromX, fromY, posX, posY);
	Util::AngleLineAbs(posX, posY, rott, moveDist, &diff.x, &diff.y);

    // Use OBB system for accurate contact point calculation
    OBB* obb = getOBB();
    Point pushSource = { fromX, fromY };
    
    // Find the closest point on the OBB to the push source
    // This is where the force is actually applied
    Point contactPoint = OBBClosestPoint(*obb, pushSource);
    
    // Get the push direction (normalized)
    double pushDirX = diff.x;
    double pushDirY = diff.y;
    double pushLen = std::sqrt(pushDirX * pushDirX + pushDirY * pushDirY);
    if (pushLen > 0.0001) {
        pushDirX /= pushLen;
        pushDirY /= pushLen;
    }
    
    // Calculate the offset from OBB center to contact point
    double rX = contactPoint.x - obb->center.x;
    double rY = contactPoint.y - obb->center.y;
    
    // Calculate torque: r × F (cross product in 2D gives scalar)
    // Torque = r.x * F.y - r.y * F.x
    // The push force magnitude is proportional to moveDist
    float torqueMag = (float)(rX * pushDirY - rY * pushDirX) * (float)moveDist;
    
    // Apply angular velocity change based on torque and inertia
    // Angular acceleration = Torque / Inertia
    float angularAccel = torqueMag / inertia;
    
    // Scale the torque effect for gameplay (can be tuned)
    float torqueScale = 50.0f;
    angularVelocity += Util::ToDeg(angularAccel) * torqueScale;

    auto line = checkCollide(hostMap->collLines, (float)moveDist, Util::RotateTowards((float)fromX, (float)fromY, (float)posX, (float)posY), 4, nullptr);
    if (!line)
    {
		OBB pushedOBB = *getOBB();
		pushedOBB.center.x += (double)diff.x;
		pushedOBB.center.y += (double)diff.y;

        bool canMove = true;
        for (auto e : hostMap->entities)
        {
            if (!e || e == this || (e->flags & EntFlag_UnderAllOtherEnts)) continue;
            if (dynamic_cast<BulletProjectile*>(e)) continue;

            OBB otherOBB = *e->getOBB();
            if (OBBIntersects(pushedOBB, otherOBB))
            {
                canMove = false;
                break;
            }
        }

        if (canMove)
        {
			posX += (double)diff.x;
			posY += (double)diff.y;
        }
    }
    physicsSlideAngle = Util::SnapAngle(Util::RotateTowards((float)fromX, (float)fromY, (float)posX, (float)posY));
    physicsSlideAmount = amount * friction;
    Sfx::Pick->play((float)posX, (float)posY);
    auto ndiff = -diff;
	if (outX) *outX = ndiff.x;
	if (outY) *outY = ndiff.y;
}

CollisionLine* Entity::checkCollide(std::vector<CollisionLine*>& collLines, float moveAmount, 
	float dirAngle, float collideDist, CollisionLine* ignore)
{
	OBB* obb = getOBB();
	
	// We check if the OBB would collide if moved by moveAmount or collideDist
	double checkDist = (double)std::max(moveAmount, collideDist);
	double dx, dy;
	Util::AngleLineAbs(0.0, 0.0, (double)dirAngle, checkDist, &dx, &dy);
	
	OBB nextOBB = *obb;
	nextOBB.center.x += (double)dx;
	nextOBB.center.y += (double)dy;

	for (CollisionLine* cl : collLines)
	{
		if (cl == ignore) continue;

		bool hit = false;
		// 1. Check if the center of the OBB tunnels through the line
		if (Util::Intersects(obb->center.x, obb->center.y, nextOBB.center.x, nextOBB.center.y, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y))
		{
			hit = true;
		}
		// 2. Check if the OBB at the next position overlaps the line
		else if (OBBIntersectsLine(nextOBB, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y, nullptr))
		{
			hit = true;
		}

		if (hit)
		{
			if (ignore == nullptr)
			{
				// Attempt to slide along the wall
				double wallAngle = Util::RotateTowards(cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y);
				
				// Determine best slide direction
				double diff = Util::AngleDifference((double)dirAngle, wallAngle);
				double slideAngle = (diff < 90.0 && diff > -90.0) ? wallAngle : wallAngle + 180.0;
				
				// Project moveAmount onto the wall to prevent unrealistic full-speed sliding
				// when hitting the wall at a steep angle.
				double angleToWall = Util::ToRad(Util::AngleDifference((double)dirAngle, slideAngle));
				double projectedMove = (double)moveAmount * std::cos(angleToWall);
				
				double cx, cy;
				Util::AngleLineAbs(0.0, 0.0, slideAngle, projectedMove, &cx, &cy);
				
				double travelAngle = Util::RepairAngle(Util::ToDeg(atan2(cy, cx)));
				
				// Recursive check to ensure we don't slide into ANOTHER wall
				auto travColl = checkCollide(collLines, projectedMove, travelAngle, collideDist, cl);
				if (!travColl)
				{
					posX += cx;
					posY += cy;
				}
			}
			return cl;
		}
	}
	return nullptr;
}

void Entity::computeInertia()
{
	// Compute moment of inertia for a rectangular body:
	// I = (1/12) * mass * (width² + height²)
	// This makes heavier and larger objects harder to spin
	float w = sizeX > 0 ? sizeX : 6.0f;  // Default fallback size
	float h = sizeY > 0 ? sizeY : 8.0f;  // Default fallback size
	
	// Use proper physics formula with scale factor for gameplay tuning
	inertia = (1.0f / 12.0f) * mass * (w * w + h * h) * inertiaScale;
	
	// Ensure minimum inertia to prevent divide-by-zero and crazy spinning
	if (inertia < 1.0f) inertia = 1.0f;
}

}
