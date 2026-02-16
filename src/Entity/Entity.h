#pragma once
#include "Game/Globals.h"
#include <vector>
#include "Game/Direction.h"

namespace nyaa {

class Entity
{
public:
	enum Flags : uint32_t
	{
		EntFlag_NoFlags = 0,
		EntFlag_MissionSpawned = 1 << 0,
		EntFlag_Dead = 1 << 1, //Map will erase this entity if true
		EntFlag_Animating = 1 << 2,
		EntFlag_UnderAllOtherEnts = 1 << 3,
		EntFlag_TPosChanged = 1 << 4,
	};

	Entity();
	
	virtual ~Entity();
	
public: //methods

	enum DamageReason
	{
		DamageReason_Unknown,
		DamageReason_Hunger,
		DamageReason_Violence,
		DamageReason_VehicleCollision,
		DamageReason_DrugAbuse,
	};

	virtual void hurt(float amount, DamageReason damageReason, Entity* damageSource = nullptr);

	virtual void playCollisionSfx();
	
	int getMapIdx();

	virtual void update();

	void getVel(float* outX, float* outY);
	
	void turnTowards(Entity* e);

	enum FaceAngle : short
	{
		FaceAngle_Left = 180,
		FaceAngle_Right = 0,
		FaceAngle_AwayFromCam = 270,
		FaceAngle_TowardsCam = 90,

		FaceAngle_Default = FaceAngle_TowardsCam,
	};

	void setFaceAngle(FaceAngle faceAngle);
	
	bool isOnScreen();

	void getHitBox(double* outLeft, double* outTop, float* outWidth, float* outHeight);
	virtual class OBB* getOBB() const;
	void resolveCollisionWith(Entity* other);

	void physicsPush(double fromX, double fromY, float amount, float* outX, float* outY);
	
	class CollisionLine* checkCollide(std::vector<class CollisionLine*>& collLines, float moveAmount, 
		float dirAngle, float collideDist, class CollisionLine* ignore);

	void setPos(double x, double y);
	void setPos(double x, double y, double z);

	void updateTPos();
	
	void setPosTile(int tx, int ty);
	
	void getChunk(int* outX, int* outY);
	
	void getTileInFront(Direction dir = Direction_Default, int* outX = nullptr, int* outY = nullptr);
	
	void getInChunkTPos(int* outX, int* outY);
	
	void getWorldHitBox(double* outLeft, double* outTop, float* outWidth, float* outHeight);
	
	void setDirAngleDeg(float degrees);

	Direction getDir() const;

	// Calculate moment of inertia based on mass and size (rectangular body)
	// Formula: I = (1/12) * mass * (width² + height²) * inertiaScale
	void computeInertia();

public: // data
	Flags flags;

	float friction;
	float mass;
	float inertia;
	float inertiaScale; // Multiplier for computed inertia (1.0 = realistic, lower = easier to spin)

	double lposX;
	double lposY;
	double lposZ;

	double posX; //horizontal
	double posY; //depth (cam faces -Y)
	double posZ; //foot height

	float sizeX;
	float sizeY;
	float sizeZ;

	float originX;
	float originY;

	int tilePosX;
	int tilePosY;
	
	int chunkPosX;
	int chunkPosY;

	float dirAngle;
	float angularVelocity; // degrees per second

	float physicsSlideAngle;
	float physicsSlideAmount;

	class Map* hostMap;
	
	std::vector<class Interaction*> interactions;
	
	float nearInteractHovArrowPosOffsetX, nearInteractHovArrowPosOffsetY;
	
#if DEBUG
	unsigned long long frameLastUpdatedOn;
#endif

	class Timer* portalActivateCooldown;
	
	class CarBase* carImInsideOf;

	std::vector<class Entity*> entitiesOnTop;

	float renderScaleX;
	float renderScaleY;
protected:
	class OBB* m_OBB;
};

inline Entity::Flags operator|(Entity::Flags lhs, Entity::Flags rhs)
{
	return static_cast<Entity::Flags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline Entity::Flags& operator|=(Entity::Flags& lhs, Entity::Flags rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

inline Entity::Flags operator&(Entity::Flags lhs, Entity::Flags rhs)
{
	return static_cast<Entity::Flags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

inline Entity::Flags operator~(Entity::Flags f)
{
	return static_cast<Entity::Flags>(~static_cast<uint32_t>(f));
}

inline Entity::Flags& operator&=(Entity::Flags& lhs, Entity::Flags rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

} // namespace nyaa
