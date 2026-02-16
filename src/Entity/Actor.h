#pragma once
#include "Game/Globals.h"
#include "Entity/Entity.h"
#include "Game/Clock.h"
#include <vector>

namespace nyaa {

class DrugTrip;

class Actor : public Entity
{
public:
	enum DeathReason
	{
		DeathReason_NotSet,
		DeathReason_Starvation,
		DeathReason_KilledByPlayer,
		DeathReason_KilledByNPC,
	};

public:
	static float HealthEnergyBasedMax;
	static float Gravity;
	static float JumpSpeed;
	static float ShiftSpeedBoostMultiplier;

public:
	Actor(std::string charTypeName,
		float bodyR, float bodyG, float bodyB, float bodyA,
		float eyeR, float eyeG, float eyeB, float eyeA,
		float hairR, float hairG, float hairB, float hairA,
		float clothesR, float clothesG, float clothesB, float clothesA,
		float tailR, float tailG, float tailB, float tailA,
		float outlineR, float outlineG, float outlineB, float outlineA);
	
	virtual ~Actor();
	
public:
	void npcFollow();
	void npcHomeWander();
	
	virtual void update() override;
	virtual class OBB* getOBB() const override;
	
	float control();

	virtual void hurt(float amount, DamageReason damageReason, Entity* damageSource = nullptr) override;
	
	void healCompletely();
	
	void healHealth();
	
	void healEnergy();
	
	void healHunger();
	
	void gotoMapInstant(std::string name);
	
	void emptyBladder(class Potty* p); /* if nullptr, empty onto floor */

	void getHandPos(double& outX, double& outY);
	
	/**
	 * @brief Makes the actor automatically walk to the specified position using pathfinding
	 * @param targetX The target X position to walk to in world coordinates
	 * @param targetY The target Y position to walk to in world coordinates
	 * @return bool True if the actor started moving, false if already at target
	 */
	bool autoWalkTo(double targetX, double targetY);

	void jump();

	void updateTrips();

	/**
	 * @brief Starts a new drug trip for this actor
	 * @param trip Pointer to the DrugTrip instance to start
	 */
	void startTrip(DrugTrip* trip);

	bool canUseItems();

public:
	float bodyR, bodyG, bodyB, bodyA;
	float eyeR, eyeG, eyeB, eyeA;
	float hairR, hairG, hairB, hairA;
	float clothesR, clothesG, clothesB, clothesA;
	float tailR, tailG, tailB, tailA;
	float outlineR, outlineG, outlineB, outlineA;
	
	bool isControllable; //DO NOT USE TO DETERMINE IF ITS THE PLAYER
	float moveAmountMax;
	float velZ;

	/* 100 good 0 bad */
	float energy;
	float health;
	float basedMeter;
	
	bool foot;//false = left foot, true = right foot

	class Timer* notMovingT;
	class Timer* energyAddT;
	class Timer* energyDelT;
	class Timer* urineAddT;
	class Timer* hungerAddT;
	class Timer* hungerDelT;
	class Timer* footT;

	/* 0 good, 100 bad */
	float hunger;
	float bladder;
	class UrinePuddle* urinePuddle;
	float spriteVOffset;

	Actor* follow;

	Clock astClock;
	class Pathfinding* ast;
	class PathfindNode* astn;
	bool reSolveAST;
	bool directPath;
	double lastTargetX, lastTargetY;

	double homePointX, homePointY;
	float homeWalkRange;
	bool isHomeWandering;
	class Timer* homeWanderT;
	double wanderTargetX, wanderTargetY;
	bool hasWanderTarget;

	class Inv* inv;
	
	class Timer* moveAccelTimer;
	
	class WalkAnim* body;
	class WalkAnim* eyes;
	class WalkAnim* hair;
	class WalkAnim* clothes;
	class WalkAnim* tail;
	class WalkAnim* outline;

	DeathReason deathReason;
	Actor* killedBy;

	DamageReason damageReason;
	Entity* damageSource;
	int footFrame;

	// Footstep snap back
	double moveStartPosX, moveStartPosY;
	bool footstepPlayedThisMove;
	bool isSnappingBack;
	double snapBackTargetX, snapBackTargetY;

	// Drug trip management
	std::vector<DrugTrip*> activeTrips;

	Direction visualDir;
	class Timer* visualTurnTimer;
	Direction visualTurnTarget;

public:
    class ActorWellbeing* wellbeing;
};

} // namespace nyaa
