#pragma once
#include "Game/Globals.h"
#include <vector>
#include <queue>

namespace nyaa {

class Map;

class PathfindNode
{
public:
	PathfindNode();

public:
	double posX, posY;
	bool visited;
	double global_goal;
	double local_goal;
	PathfindNode* parent;
	PathfindNode* child;
	std::vector<PathfindNode*> nearby;
	int id;
};

struct NodeComparator {
	bool operator()(const PathfindNode* a, const PathfindNode* b) const {
		return a->global_goal > b->global_goal;
	}
};

class Pathfinding
{
public:
	Pathfinding();
	~Pathfinding();

public:
	void loadNodes(Map* map, double fromX, double fromY, double destX, double destY);
	
	void solve();

	bool valid();
	
private:
	bool hasLineOfSight(double fromX, double fromY, double toX, double toY, Map* map);
	
public:
	PathfindNode* nodes;
	PathfindNode* startNode;
	PathfindNode* endNode;
	size_t nodeCount;
	Map* cachedMap;
};


}