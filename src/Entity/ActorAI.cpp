#include "Entity/Actor.h"
#include "Game/Game.h"
#include <SFML/Graphics.hpp>
#include "Map/Map.h"
#include "Map/CollisionLine.h"
#include "Map/Pathfinding.h"
#include "Entity/BulletProjectile.h"
#include "Entity/Door.h"
#include "Game/Util.h"
#include "Screen/InGameScreen.h"
#include "Game/Timer.h"
#include "Game/OBB.h"
#include <cstdlib>

namespace nyaa {

void Actor::npcFollow()
{
	// Stop following when in dialogue
	if (IGS->dbox.showing)
	{
		flags &= ~EntFlag_Animating;
		return;
	}

	if (follow)
	{
		Point apos = { follow->posX, follow->posY };
		//Vector2i& atpos = follow->tilePos;
		float mo = moveAmountMax * (G->frameDeltaMillis / (1000 / Game::FPSConstant) );
		float collideDist = 8/*  * (inst->frameDeltaMillis / (1000 / Game::FPSConstant)) */;
		if (!ast || reSolveAST)
		{
			if (Util::Dist(posX, posY, apos.x, apos.y) > (float)hostMap->TileSizePixels)
			{
				float directAngle = Util::RotateTowards(this->posX, this->posY, apos.x, apos.y);
				if (directPath)
				{
					flags |= EntFlag_Animating;
					dirAngle = directAngle;
					CollisionLine* dcl = (flags & EntFlag_UnderAllOtherEnts) ? nullptr : checkCollide(hostMap->collLines, mo, directAngle, collideDist, nullptr);
					if (!dcl)
					{
						Point add = {};
						Util::AngleLineRel(this->posX, this->posY, dirAngle, mo, &add.x, &add.y);
						dirAngle = directAngle;
						posX += add.x - posX;
						posY += add.y - posY;
					}
					else
					{
						directPath = false;
						flags &= ~EntFlag_Animating;
					}
				}
				else
				{
					flags |= EntFlag_Animating;
					dirAngle = directAngle;
					bool lineOfSightCollided = false;
					float losDist = Util::Dist(this->posX, this->posY, apos.x, apos.y);
					if (losDist > 4.0f)
					{
						OBB losOBB;
						losOBB.center = { (this->posX + apos.x) * 0.5f, (this->posY + apos.y) * 0.5f };
						losOBB.angle = Util::RotateTowards(this->posX, this->posY, apos.x, apos.y);
						losOBB.halfExtents = { losDist * 0.5f, 6.0f }; // 6.0f half-thickness margin

						for (CollisionLine* cl : hostMap->collLines)
						{
							if (OBBIntersectsLine(losOBB, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y, nullptr))
							{
								lineOfSightCollided = true;
								break;
							}
						}
					}
					if (lineOfSightCollided)
					{
						// Reduce pathfinding frequency for better performance
						if (astClock.getElapsedMilliseconds() >= 500)
						{
							astClock.restart();
							// Only recalculate if target moved significantly
							if (!ast || Util::Dist(lastTargetX, lastTargetY, apos.x, apos.y) > 20.0f)
							{
								lastTargetX = apos.x;
								lastTargetY = apos.y;
								if(ast) delete ast;
								ast = new Pathfinding();
								ast->loadNodes(hostMap, this->posX, this->posY, apos.x, apos.y);
								ast->solve();
								astn = ast->startNode;
								reSolveAST = false;
							}
						}
					}
					else
					{
						directPath = true;
					}
				}
			}
			else
			{
				directPath = false;
				flags &= ~EntFlag_Animating;
			}
		}
		else
		{
			if (astn)
			{
				Point to = { astn->posX, astn->posY };
				if (Util::Dist(posX, posY, to.x, to.y) > 1.0)
				{
					dirAngle = Util::RotateTowards(this->posX, this->posY, to.x, to.y);
					flags |= EntFlag_Animating;
					auto cl = (flags & EntFlag_UnderAllOtherEnts) ? nullptr : checkCollide(hostMap->collLines, mo, dirAngle, collideDist, nullptr);
					if (!cl)
					{
						auto add = Point{};
						Util::AngleLineRel(this->posX, this->posY, (double)dirAngle, (double)mo, &add.x, &add.y);
						posX = add.x;
						posY = add.y;
					}
				}
				else
				{
					astn = astn->child;
				}
			}
			else
			{
				if (ast)
				{
					reSolveAST = true;
					flags &= ~EntFlag_Animating;
				}
			}
		}
	}
}

bool Actor::autoWalkTo(double targetX, double targetY)
{
	if(posX == targetX && posY == targetY)
	{
		return false;
	}
	Point targetPos = { targetX, targetY };
    // If we have a path, follow it
    if (astn && ast->valid()) {
        flags |= EntFlag_Animating;
        
        // Get next node position
		Point nextPos = { astn->posX, astn->posY };
        
        // Calculate direction and move
        float moveAmount = moveAmountMax * (G->frameDeltaMillis / (1000.0f / Game::FPSConstant));
        float angle = Util::RotateTowards(posX, posY, nextPos.x, nextPos.y);
        dirAngle = angle;
        
        // Move towards next node using checkCollide for wall interactions
        float collideDist = 8.0f;
        auto cl = checkCollide(hostMap->collLines, moveAmount, angle, collideDist, nullptr);
        if (!cl)
        {
            Point moveVec = {};
            Util::AngleLineRel(posX, posY, (double)angle, (double)moveAmount, &moveVec.x, &moveVec.y);
            posX = moveVec.x;
            posY = moveVec.y;
        }
        
        // Check if we've reached the current node
        float reachThreshold = 2.0f;
        if (Util::Dist(posX, posY, nextPos.x, nextPos.y) < reachThreshold) {
            // Move to next node in path
            if (astn->child) {
                astn = astn->child;
            } else {
                // Snap to exact final position
                posX = nextPos.x;
				posY = nextPos.y;
                // Reached the end of the path
                delete ast;
                ast = nullptr;
                astn = nullptr;
                flags &= ~EntFlag_Animating;
                return false;
            }
        }
        
        return true;
    }

    // If we get here, pathfinding failed
    flags &= ~EntFlag_Animating;
    return false;
}

void Actor::npcHomeWander()
{
	if (!isHomeWandering)
	{
		hasWanderTarget = false;
		return;
	}

	// Stop wandering when in dialogue
	if (IGS->dbox.showing)
	{
		hasWanderTarget = false;
		flags &= ~EntFlag_Animating;
		return;
	}

	float mo = moveAmountMax * (G->frameDeltaMillis / (1000 / Game::FPSConstant));
	float collideDist = 8.0f;

	if (!hasWanderTarget)
	{
		homeWanderT->update();
		flags &= ~EntFlag_Animating;
		if (homeWanderT->millis() > 2000 + (std::rand() % 4000))
		{
			float angle = (float)(std::rand() % 360);
			float dist = (homeWalkRange > 0.1f) ? (float)(std::rand() % (int)homeWalkRange) : 0.0f;
			double tx, ty;
			Util::AngleLineAbs(homePointX, homePointY, (double)angle, (double)dist, &tx, &ty);

			wanderTargetX = tx;
			wanderTargetY = ty;
			hasWanderTarget = true;
			reSolveAST = true;
			homeWanderT->zero();
		}
	}
	else
	{
		Point apos = { wanderTargetX, wanderTargetY };
		if (!ast || reSolveAST)
		{
			if (Util::Dist(posX, posY, apos.x, apos.y) > 4.0f)
			{
				float directAngle = Util::RotateTowards(this->posX, this->posY, apos.x, apos.y);
				if (directPath)
				{
					flags |= EntFlag_Animating;
					dirAngle = directAngle;
					CollisionLine* dcl = (flags & EntFlag_UnderAllOtherEnts) ? nullptr : checkCollide(hostMap->collLines, mo, directAngle, collideDist, nullptr);
					if (!dcl)
					{
						Point add = {};
						Util::AngleLineRel(this->posX, this->posY, dirAngle, mo, &add.x, &add.y);
						dirAngle = directAngle;
						posX += add.x - posX;
						posY += add.y - posY;
					}
					else
					{
						directPath = false;
						flags &= ~EntFlag_Animating;
					}
				}
				else
				{
					flags |= EntFlag_Animating;
					dirAngle = directAngle;
					bool lineOfSightCollided = false;
					float losDist = Util::Dist(this->posX, this->posY, apos.x, apos.y);
					if (losDist > 4.0f)
					{
						OBB losOBB;
						losOBB.center = { (this->posX + apos.x) * 0.5f, (this->posY + apos.y) * 0.5f };
						losOBB.angle = Util::RotateTowards(this->posX, this->posY, apos.x, apos.y);
						losOBB.halfExtents = { losDist * 0.5f, 6.0f }; // 6.0f half-thickness margin

						for (CollisionLine* cl : hostMap->collLines)
						{
							if (OBBIntersectsLine(losOBB, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y, nullptr))
							{
								lineOfSightCollided = true;
								break;
							}
						}
					}
					if (lineOfSightCollided)
					{
						// Reduce pathfinding frequency for better performance
						if (astClock.getElapsedMilliseconds() >= 500)
						{
							astClock.restart();
							// Only recalculate if target moved significantly
							if (!ast || Util::Dist(lastTargetX, lastTargetY, apos.x, apos.y) > 20.0f)
							{
								lastTargetX = apos.x;
								lastTargetY = apos.y;
								if (ast) delete ast;
								ast = new Pathfinding();
								ast->loadNodes(hostMap, this->posX, this->posY, apos.x, apos.y);
								ast->solve();
								astn = ast->startNode;
								reSolveAST = false;

								if (!astn)
								{
									// Pathfinding failed, maybe point is inside a wall?
									hasWanderTarget = false;
									flags &= ~EntFlag_Animating;
									homeWanderT->zero();
								}
							}
						}
					}
					else
					{
						directPath = true;
					}
				}
			}
			else
			{
				hasWanderTarget = false;
				flags &= ~EntFlag_Animating;
				homeWanderT->zero();
			}
		}
		else
		{
			if (astn)
			{
				Point to = { astn->posX, astn->posY };
				if (Util::Dist(posX, posY, to.x, to.y) > 2.0)
				{
					dirAngle = (float)Util::RotateTowards(this->posX, this->posY, to.x, to.y);
					flags |= EntFlag_Animating;
					auto cl = (flags & EntFlag_UnderAllOtherEnts) ? nullptr : checkCollide(hostMap->collLines, mo, dirAngle, collideDist, nullptr);
					if (!cl)
					{
						auto add = Point{};
						Util::AngleLineRel(this->posX, this->posY, (double)dirAngle, (double)mo, &add.x, &add.y);
						posX += add.x - posX;
						posY += add.y - posY;
					}
				}
				else
				{
					astn = astn->child;
				}
			}
			else
			{
				hasWanderTarget = false;
				flags &= ~EntFlag_Animating;
				homeWanderT->zero();
			}
		}
	}
}

}
