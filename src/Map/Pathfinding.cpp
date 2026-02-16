#include "Map/Pathfinding.h"
#include "Map/Map.h"
#include "Game/Util.h"
#include <limits>
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include "Game/OBB.h"

namespace nyaa {

Pathfinding::Pathfinding()
	:nodes(nullptr)
	,startNode(nullptr)
	,endNode(nullptr)
	,nodeCount(0)
	,cachedMap(nullptr)
{
}

bool Pathfinding::hasLineOfSight(double fromX, double fromY, double toX, double toY, Map* map)
{
	// Quick distance check - skip very close nodes
	double distSq = (fromX - toX) * (fromX - toX) + (fromY - toY) * (fromY - toY);
	if (distSq < 4.0) return true;
	
	double dist = sqrt(distSq);
	
	// Create an OBB representing the 'thick' path to account for actor width
	OBB pathOBB;
	pathOBB.center = { (fromX + toX) * 0.5, (fromY + toY) * 0.5 };
	pathOBB.angle = (float)Util::RotateTowards(fromX, fromY, toX, toY);
	// halfExtents.x is half the length, halfExtents.y is half the thickness
	// We use 6.0 as half-thickness (12.0 total width) to provide a safe margin for the actor (approx 12.0 wide)
	pathOBB.halfExtents = { dist * 0.5, 6.0 };
	
	// Only check collision lines that could intersect the path
	double minX = std::min(fromX, toX) - 15.0;
	double maxX = std::max(fromX, toX) + 15.0;
	double minY = std::min(fromY, toY) - 15.0;
	double maxY = std::max(fromY, toY) + 15.0;
	
	for (auto cl : map->collLines)
	{
		// Quick bounds check
		if (std::max(cl->p1.x, cl->p2.x) < minX || std::min(cl->p1.x, cl->p2.x) > maxX ||
			std::max(cl->p1.y, cl->p2.y) < minY || std::min(cl->p1.y, cl->p2.y) > maxY)
			continue;
			
		// Check for intersection with the thick path
		if (OBBIntersectsLine(pathOBB, (float)cl->p1.x, (float)cl->p1.y, (float)cl->p2.x, (float)cl->p2.y, nullptr))
			return false;
	}
	return true;
}

void Pathfinding::loadNodes(Map* map, double fromX, double fromY, double destX, double destY)
{
	cachedMap = map;
	
	// Check if we can walk directly to destination
	if (hasLineOfSight(fromX, fromY, destX, destY, map))
	{
		// Create minimal 2-node path for direct movement
		nodeCount = 2;
		nodes = new PathfindNode[nodeCount];
		nodes[0].id = 0;
		nodes[0].posX = fromX;
		nodes[0].posY = fromY;
		nodes[1].id = 1;
		nodes[1].posX = destX;
		nodes[1].posY = destY;
		nodes[0].nearby.push_back(&nodes[1]);
		startNode = &nodes[0];
		endNode = &nodes[1];
		return;
	}
	
	size_t mapPathNodeCount = map->pathFindingNodePoints.size();
	nodeCount = mapPathNodeCount + 2;
	nodes = new PathfindNode[nodeCount];
 
	// Initialize nodes with IDs
	for (size_t i = 0; i < nodeCount; i++)
	{
		auto& n = nodes[i];
		n.id = i;
		if (i < mapPathNodeCount)
		{
			auto& pi = map->pathFindingNodePoints[i];
			n.posX = pi.x;
			n.posY = pi.y;
		}
	}
 
	startNode = &nodes[mapPathNodeCount];
	startNode->posX = fromX;
	startNode->posY = fromY;
	endNode = &nodes[mapPathNodeCount + 1];
	endNode->posX = destX;
	endNode->posY = destY;
 
	// Build connectivity graph with spatial optimization
	const double maxConnectionDist = 150.0; // Limit connection distance
	
	for (size_t i = 0; i < nodeCount; ++i)
	{
		PathfindNode* n1 = &nodes[i];
		n1->nearby.reserve(8); // Reserve space to avoid reallocations
		
		for (size_t j = i + 1; j < nodeCount; ++j)
		{
			PathfindNode* n2 = &nodes[j];
			
			// Skip if too far apart
			double distSq = (n1->posX - n2->posX) * (n1->posX - n2->posX) + 
						   (n1->posY - n2->posY) * (n1->posY - n2->posY);
			if (distSq > maxConnectionDist * maxConnectionDist)
				continue;
			
			if (hasLineOfSight(n1->posX, n1->posY, n2->posX, n2->posY, map))
			{
				n1->nearby.push_back(n2);
				n2->nearby.push_back(n1);
			}
		}
	}
}

void Pathfinding::solve()
{
	// Fast distance calculation without sqrt
	auto distance = [](PathfindNode* a, PathfindNode* b) -> double
	{
		double dx = a->posX - b->posX;
		double dy = a->posY - b->posY;
		return sqrt(dx * dx + dy * dy);
	};

	// Use priority queue instead of sorting list every iteration
	std::priority_queue<PathfindNode*, std::vector<PathfindNode*>, NodeComparator> openSet;
	std::unordered_set<int> inOpenSet;

	startNode->local_goal = 0.0f;
	startNode->global_goal = distance(startNode, endNode);
	openSet.push(startNode);
	inOpenSet.insert(startNode->id);

	while (!openSet.empty())
	{
		PathfindNode* current = openSet.top();
		openSet.pop();
		inOpenSet.erase(current->id);

		if (current->visited) continue;
		current->visited = true;

		if (current == endNode) break;

		for (PathfindNode* neighbor : current->nearby)
		{
			if (neighbor->visited) continue;

			float tentativeG = current->local_goal + distance(current, neighbor);

			if (tentativeG < neighbor->local_goal)
			{
				neighbor->parent = current;
				neighbor->local_goal = tentativeG;
				neighbor->global_goal = tentativeG + distance(neighbor, endNode);

				if (inOpenSet.find(neighbor->id) == inOpenSet.end())
				{
					openSet.push(neighbor);
					inOpenSet.insert(neighbor->id);
				}
			}
		}
	}

	// Build path chain
	PathfindNode* n = endNode;
	while (n && n->parent)
	{
		n->parent->child = n;
		n = n->parent;
	}
}

bool Pathfinding::valid()
{
	return endNode && endNode->parent != nullptr;
}

Pathfinding::~Pathfinding()
{
	if (nodes)
	{
		delete[] nodes;
		nodes = nullptr;
	}
}

PathfindNode::PathfindNode()
	:posX(0.0)
	,posY(0.0)
	,visited(false)
	,global_goal(std::numeric_limits<double>::infinity())
	,local_goal(std::numeric_limits<double>::infinity())
	,parent(nullptr)
	,child(nullptr)
	,id(-1)
{
	nearby.reserve(8);
}

}