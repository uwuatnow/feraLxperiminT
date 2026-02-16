#include "Entity/Entity.h"
#include "Game/Util.h"
#include "Entity/BulletProjectile.h"
#include "Screen/InGameScreen.h"
#include "Entity/CarBase.h"
#include "Entity/Actor.h"
#include "Entity/Interaction.h"
#include "Game/Game.h"
#include "Game/Sfx.h"
#include "Map/Map.h"
#include "Game/Timer.h"
#include <cassert>
#include "Game/OBB.h"

namespace nyaa {

void Entity::setFaceAngle(FaceAngle faceAngle)
{
	faceAngle = (FaceAngle)((int)faceAngle % 360);
	dirAngle = faceAngle;
}

Entity::Entity()
	:flags(EntFlag_NoFlags)
	,friction(1.0f)
	,mass(1.0f)
	,inertia(10.0f)
	,inertiaScale(1.0f)
	,lposX(0)
	,lposY(0)
	,lposZ(0)
	,posX(0)
	,posY(0)
	,posZ(0)
	,sizeX(0)
	,sizeY(0)
	,sizeZ(0)
	,originX(0)
	,originY(0)
	,tilePosX(0)
	,tilePosY(0)
	,chunkPosX(0)
	,chunkPosY(0)
	,dirAngle(0.0f)
	,angularVelocity(0.0f)
	,physicsSlideAngle(0.0f)
	,physicsSlideAmount(0.0f)
	,hostMap(nullptr)
	,interactions()
	,nearInteractHovArrowPosOffsetX(0.0f)
	,nearInteractHovArrowPosOffsetY(0.0f)
#if DEBUG
	,frameLastUpdatedOn(-1)
#endif
	,portalActivateCooldown(new Timer())
	,carImInsideOf(nullptr)
	,entitiesOnTop()
	,renderScaleX(1)
	,renderScaleY(1)
	,m_OBB(new OBB())
{

}

Entity::~Entity() 
{
	delete portalActivateCooldown;
	delete m_OBB;
	//std::cout << "Entity dtor" << std::endl;
}

void Entity::getVel(float* outX, float* outY)
{
	// Calculate velocity as (current position - last position) / dt
	double dx = posX - lposX;
	double dy = posY - lposY;
	float dt = G->frameDeltaMillis / 1000.0f;
	if (dt > 0.0001f) {
		if (outX) *outX = (float)(dx / dt);
		if (outY) *outY = (float)(dy / dt);
	} else {
		if (outX) *outX = 0;
		if (outY) *outY = 0;
	}
}

void Entity::playCollisionSfx()
{
	Sfx::Pick->play((float)posX, (float)posY);
}

int Entity::getMapIdx()
{
	assert(hostMap);
	return hostMap->index;
}

void Entity::update()
{
	portalActivateCooldown->update();
#if DEBUG
	//checks if entity was updated more than once per frame
	assert(frameLastUpdatedOn != G->framesPassed);
	frameLastUpdatedOn = G->framesPassed;
#endif
	float dt = G->frameDeltaMillis / 1000.0f;
	Point slide = {};
	Util::AngleLineAbs(0.0, 0.0, (double)physicsSlideAngle, (double)physicsSlideAmount * (double)dt, &slide.x, &slide.y);
	if (flags & EntFlag_UnderAllOtherEnts)
	{
		posX += slide.x;
		posY += slide.y;
	}
	else
	{
		auto cl = checkCollide(hostMap->collLines, physicsSlideAmount * dt, physicsSlideAngle, 4, nullptr);
		if (!cl)
		{
			posX += slide.x;
			posY += slide.y;
		}
	}
	
	// --- Wall Overlap Recovery ---
	if (!(flags & EntFlag_UnderAllOtherEnts))
	{
		OBB* myOBB = getOBB();
		for (auto cl : hostMap->collLines) {
			Point mtv;
			if (OBBIntersectsLine(*myOBB, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y, &mtv)) {
				posX += mtv.x;
				posY += mtv.y;
				myOBB = getOBB(); // Update for next line check
			}
		}
	}

	// Damping (was 0.04 per frame at 60fps) - only on ground to allow air momentum
	if (posZ == 0.0f) {
		physicsSlideAmount /= (1.0f + 2.4f * dt);
	}
	
	// resolveCollisionWith is now called from Map::update
	// resolveEntityCollisions();

	/*if(IGS->curMission)
		IGS->curMission->onEntityTick(this);*/

	lposX = posX; // Store position for next frame's velocity calculation
	lposY = posY;
	lposZ = posZ;

	// Move entities on top together if speeds match
	float deltaX = posX - lposX;
	float deltaY = posY - lposY;
	float deltaZ = posZ - lposZ;
	if (dt > 0) {
		float thisVelX = deltaX / dt;
		float thisVelY = deltaY / dt;
		float thisVelZ = deltaZ / dt;
		for (auto top : entitiesOnTop) {
			if (top->flags & EntFlag_Dead) continue;
			float topVelX, topVelY;
			top->getVel(&topVelX, &topVelY);
			float topVelZ = (top->posZ - top->lposZ) / dt;
			if (std::abs(topVelX - thisVelX) < 5.0f && std::abs(topVelY - thisVelY) < 5.0f && std::abs(topVelZ - thisVelZ) < 5.0f) {
				top->posX += deltaX;
				top->posY += deltaY;
				top->posZ += deltaZ;
			}
		}
	}

	// Apply angular velocity
	dirAngle += angularVelocity * dt;
	if (dirAngle < 0) dirAngle += 360.f;
	if (dirAngle >= 360.f) dirAngle -= 360.f;
	
	// Apply angular damping
	angularVelocity /= (1.0f + 1.5f * dt); // was 3.0f
}

void Entity::turnTowards(Entity* e)
{
	dirAngle = Util::RotateTowards(posX, posY, e->posX, e->posY);
}

bool Entity::isOnScreen()
{
	const auto & v = IGS->worldView;
	auto& c = v.getCenter(), & s = v.getSize();
	auto over = 1.1f;
	auto sd = sf::Vector2f((s.x / 2) * over,(s.y / 2) * over);
	auto vp = sf::FloatRect(c.x - sd.x, c.y - sd.y, s.x * over, s.y * 1.25f);
	return vp.contains(sf::Vector2f{ (float)posX,(float)posY });
}

void Entity::setPos(double x, double y)
{
	posX = x;
	posY = y;
}

void Entity::setPos(double x, double y, double z)
{
	posX = x;
	posY = y;
	posZ = z;
}

void Entity::updateTPos()
{
	auto beftpos = sf::Vector2i{ tilePosX, tilePosY };
	tilePosX = (int)(posX / (double)hostMap->TileSizePixels);
	tilePosY = (int)(posY / (double)hostMap->TileSizePixels);
	if (posX < 0) tilePosX -= 1;
	if (posY < 0) tilePosY -= 1;
	chunkPosX = (int)(posX / (double)MapChunk::SizePixels);
	chunkPosY = (int)(posY / (double)MapChunk::SizePixels);
	if (posX < 0) chunkPosX -= 1;
	if (posY < 0) chunkPosY -= 1;
	if (beftpos != sf::Vector2i{ tilePosX, tilePosY } && (flags & EntFlag_Animating))
		flags |= EntFlag_TPosChanged;
}

void Entity::setPosTile(int tx, int ty)
{
	flags &= ~EntFlag_TPosChanged;
	posX = tx * (float)hostMap->TileSizePixels + ((float)hostMap->TileSizePixels / 2);
	posY = ty * (float)hostMap->TileSizePixels + ((float)hostMap->TileSizePixels / 2);
	updateTPos();
}

void Entity::getChunk(int* outX, int* outY)
{
	int ch_s = (int)Map::TileSizePixels * (int)MapChunk::SizeTiles;
	if(outX) *outX = posX / ch_s;
	if(outY) *outY = posY / ch_s;
}

void Entity::getTileInFront(Direction dir, int* outX, int* outY)
{
	sf::Vector2i ret = { tilePosX, tilePosY };
	if (dir == Direction_Default)dir = this->getDir();
	if (dir == Direction_Down)
	{
		ret = sf::Vector2i(posX / (float)Map::TileSizePixels, (posY + ((float)Map::TileSizePixels / 2)) / (float)Map::TileSizePixels);
	}
	else if (dir == Direction_Left)
	{
		ret = sf::Vector2i((posX - ((float)Map::TileSizePixels / 2)) / (float)Map::TileSizePixels, posY / (float)Map::TileSizePixels);
	}
	else if (dir == Direction_Up)
	{
		ret = sf::Vector2i(posX / (float)Map::TileSizePixels, (posY - ((float)Map::TileSizePixels / 2)) / (float)Map::TileSizePixels);
	}
	else if (dir == Direction_Right)
	{
		ret = sf::Vector2i((posX + ((float)Map::TileSizePixels / 2)) / (float)Map::TileSizePixels, posY / (float)Map::TileSizePixels);
	}
	if (posX < 0)
	{
		ret.x -= 1;
	}
	if (posY < 0)
	{
		ret.y -= 1;
	}

	if (outX) *outX = ret.x;
	if (outY) *outY = ret.y;
}

void Entity::getInChunkTPos(int* outX, int* outY)
{
	auto ret = sf::Vector2i(
		abs(tilePosX) % MapChunk::SizeTiles,
		abs(tilePosY) % MapChunk::SizeTiles);
	if (posX < 0)
		ret.x = MapChunk::SizeTiles - ret.x - 1;
	if (posY < 0)
		ret.y = MapChunk::SizeTiles - ret.y - 1;
	
	if (outX) *outX = ret.x;
	if (outY) *outY = ret.y;
}

void Entity::getWorldHitBox(double* outLeft, double* outTop, float* outWidth, float* outHeight)
{
	if(outLeft) *outLeft = posX - (double)originX;
	if(outTop) *outTop = posY - (double)originY;
	if(outWidth) *outWidth = sizeX;
	if(outHeight) *outHeight = sizeY;
}

void Entity::setDirAngleDeg(float degrees)
{
	dirAngle = degrees;
}

Direction Entity::getDir() const
{
	return Util::DirFromAngle(dirAngle);
}

void Entity::hurt(float amount, DamageReason damageReason, Entity* damageSource)
{
	
}

} // namespace nyaa
