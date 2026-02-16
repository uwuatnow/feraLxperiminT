#pragma once
#include "Game/Globals.h"

#include <vector>
#include <list>
#include <string>

#include "Map/MapTile.h"
#include "Map/CollisionLine.h"
#include "Map/Portal.h"
#include "Map/MapChunk.h"

namespace nyaa {

class Sheet;
class Entity;
class Interaction;
class MapRenderer;

class Map
{	
public:
	static const char* MapsDir;
	
public: //enums
	enum : int
	{
		TileSizePixels = 16,
		CustomNameLen = 100
	};
		
	enum Type
	{
	   Type_Indoors,
	   Type_Outdoors
	};

public:
	Map(std::string name, Sheet& sheet);
	
	~Map();
	
public: //methods
	void save();
	
	bool canAddToPathNodeList(double pX, double pY, CollisionLine* ignore);
	
	bool doesLineIntersectCollisionLine(double p1X, double p1Y, double p2X, double p2Y);
	
	void recalcPathfindingNodes();
	
	void update();
	
	bool removeEnt(Entity* e);
	
	Portal* getPortal(std::string name);
	
	Portal* addPortal(int tx, int ty);
	
	void delPortal(Portal* port);
	
	MapChunk* addChunk(int x, int y);
	
	void unloadAllChunks();
	
	void getTILE(int x, int y, int layer, MapTile** out);
	
	MapChunk* getChunk(int x, int y, bool create = false);
	
	void moveEntHere(Entity* e, int posX, int posY);
	
	std::vector<Entity*> getMissionSpawnedEnts();
	
	void addEnt(Entity* e, bool missionSpawned = false);
	
	const std::vector<Entity*>& getEntities() const { return entities; }

private:
	std::vector<Entity*> entities;
	std::vector<Entity*> ents_to_add;
	friend class Actor;
	friend class BulletProjectile;
	friend class Car;
	friend class Entity;
	friend class Mission;
	friend class DefaultMapRenderer;
	friend class New3DRenderer;

public: //data
	struct TileRendRect { float left, top, width, height; };
	std::vector<TileRendRect> RectOfTilesRendered;
	Type type;
	int index;
	std::string customName;
	std::string name;
	std::string folderLoc;
	std::string mapConfLoc;
	std::string lightsConfLoc;
	std::string buildingsConfLoc;
	int spawn_tileX, spawn_tileY;
	int lastPortalSpawnInTileX, lastPortalSpawnInTileY;
	Sheet& sheet;
	std::vector<MapChunk*> chunks;
	std::list<Portal> portals;
	std::vector<CollisionLine*> collLines;
	bool needPathfindingNodesRecalc;
	struct PathFindNode { double x, y; };
	std::vector<PathFindNode> pathFindingNodePoints;
	class WorldCrime* worldCrime;
	std::vector<class Building*> buildings;
};


}
