#include "Entity/Actor.h"
#include "Inventory/Inventory.h"
#include "Texture/WalkAnim.h"
#include "Entity/UrinePuddle.h"
#include "Map/Pathfinding.h"
#include "Entity/ActorWellbeing.h"

namespace nyaa {

Actor::Actor(
	std::string charTypeName, 
	float bodyR, float bodyG, float bodyB, float bodyA,
	float eyeR, float eyeG, float eyeB, float eyeA,
	float hairR, float hairG, float hairB, float hairA,
	float clothesR, float clothesG, float clothesB, float clothesA,
	float tailR, float tailG, float tailB, float tailA,
	float outlineR, float outlineG, float outlineB, float outlineA
)
	:Entity()
	,bodyR(bodyR), bodyG(bodyG), bodyB(bodyB), bodyA(bodyA)
	,eyeR(eyeR), eyeG(eyeG), eyeB(eyeB), eyeA(eyeA)
	,hairR(hairR), hairG(hairG), hairB(hairB), hairA(hairA)
	,clothesR(clothesR), clothesG(clothesG), clothesB(clothesB), clothesA(clothesA)
	,tailR(tailR), tailG(tailG), tailB(tailB), tailA(tailA)
	,outlineR(outlineR), outlineG(outlineG), outlineB(outlineB), outlineA(outlineA)
	,isControllable(false)
	,moveAmountMax(1.25f)
	,velZ(0.0f)
	,energy(HealthEnergyBasedMax)
	,health(HealthEnergyBasedMax * 0.8)
	,basedMeter(0)
	,foot(false)
	,notMovingT(new Timer())
	,energyAddT(new Timer())
	,energyDelT(new Timer())
	,urineAddT(new Timer())
	,hungerAddT(new Timer())
	,hungerDelT(new Timer())
	,footT(new Timer())
	,hunger(25.0f)
	,bladder(0.0f)
	,urinePuddle(nullptr)
	,spriteVOffset(5.0f)
	,follow(nullptr)
	,ast(nullptr)
	,astn(nullptr)
	,reSolveAST(false)
	,directPath(false)
	,lastTargetX(0.0f)
	,lastTargetY(0.0f)
	,homePointX(0.0f)
	,homePointY(0.0f)
	,homeWalkRange(100.0f)
	,isHomeWandering(false)
	,homeWanderT(new Timer())
	,wanderTargetX(0.0f)
	,wanderTargetY(0.0f)
	,hasWanderTarget(false)
	,inv(new Inv(this))
	,moveAccelTimer(new Timer())
	,body(new WalkAnim(charTypeName + "body", this))
	,eyes(new WalkAnim(charTypeName + "eyes", this))
	,hair(new WalkAnim(charTypeName + "hair", this))
	,clothes(new WalkAnim(charTypeName + "clothes", this))
	,tail(new WalkAnim(charTypeName + "tail", this))
	,outline(new WalkAnim(charTypeName + "outline", this))
	,deathReason(DeathReason_NotSet)
	,killedBy(nullptr)
	,damageReason(DamageReason_Unknown)
	,damageSource(nullptr)
	,footFrame(-1)
	,moveStartPosX(0.0)
	,moveStartPosY(0.0)
	,footstepPlayedThisMove(false)
	,isSnappingBack(false)
	,snapBackTargetX(0.0)
	,snapBackTargetY(0.0)
	,wellbeing(new ActorWellbeing())
{
	friction = 0.23f;
	mass = 70.0f;
	originX = 0.5f;
	originY = 1.0f;
	nearInteractHovArrowPosOffsetX = 0.0f;
	nearInteractHovArrowPosOffsetY = -28.0f;
	sizeX = (float)body->tex.getSize().x / (float)body->columns;
	sizeY = (float)body->tex.getSize().y / (float)body->rows;
	
	// Compute inertia from mass and size for realistic rotation physics
	inertiaScale = 1.0f; // Normal human resistance to spinning
	computeInertia();

	sizeZ = 16.0f; // Actor height

	visualDir = getDir();
	visualTurnTimer = new Timer();
}

Actor::~Actor()
{
	delete notMovingT;
	delete energyAddT;
	delete energyDelT;
	delete urineAddT;
	delete hungerAddT;
	delete hungerDelT;
	delete footT;
	delete homeWanderT;
	delete moveAccelTimer;
	delete visualTurnTimer;
	delete wellbeing;
	if (urinePuddle)
	{
		urinePuddle->done = true;
		urinePuddle->wetter = nullptr;
	}
	if(ast)
	{
		delete ast;
	}

	delete body;
	delete eyes;
	delete hair;
	delete clothes;
	delete tail;
	delete outline;
	delete inv;
}

}