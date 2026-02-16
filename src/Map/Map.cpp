#include "Map/Map.h"
#include "Screen/InGameScreen.h"
#include <fstream>
#include "Game/Util.h"
#include "Entity/Actor.h"
#include "Game/Game.h"
#include "Entity/Microwave.h"
#include "Entity/Road.h"
#include "Entity/Light.h"
#include <cstring>
#include "Mission/Mission.h"
#include "Map/MapFactory.h"
#include <cassert>
#include <iostream>
#include "Game/Sfx.h"
#include "Game/Mouse.h"
#include <SFML/Graphics/CircleShape.hpp>
#include "Map/New3DRenderer.h"
#include "Game/WorldCrime.h"
#include "Map/Building.h"
#include "Entity/BuildingWallEntity.h"

namespace nyaa {

MapTile::MapTile()
	:rotation(TileFlags_None)
	,colorR(255)
	,colorG(255)
	,colorB(255)
	,colorA(255)
	,id(0)
{

}

const char* Map::MapsDir = "./maps/";

Map::Map(std::string name, Sheet& sheet)
	:type(Type_Indoors)
	,index(-1)
	,customName(CustomNameLen, '\0')
	,name(name)
	,folderLoc(std::string(MapsDir) + name + "/")
	,mapConfLoc(folderLoc + "map.txt")
	,lightsConfLoc(folderLoc + "lights.txt")
	,buildingsConfLoc(folderLoc + "buildings.txt")
	,spawn_tileX(0)
	,spawn_tileY(0)
	,lastPortalSpawnInTileX(-1)
	,lastPortalSpawnInTileY(-1)
	,sheet(sheet)
	,needPathfindingNodesRecalc(true)
	,worldCrime(new WorldCrime())
{
	index = -1;
	Util::MakeDir(folderLoc);

	FILE* f = fopen(mapConfLoc.c_str(), "r");
	if (f)
	{
		char buffer[8192];
		enum Mode
		{
			Header, Portals, CollLines, Roads
		};
		int mode = Header;
		Portal* portal = nullptr;
		int ptx, pty;
		sf::Vector2f clp1, clp2;
		Road* road = nullptr;
		int li = 0, sl;
		while (fgets(buffer, sizeof(buffer), f) != NULL)
		{
			if (buffer[0] == '@')
			{
				mode++;
				li = 0;
				continue;
			}
			sl = strlen(buffer);
			if (sl > 0) sl--;
			buffer[sl] = '\0'; //replace \n with \0
			switch (mode)
			{
			case Header:
				switch (li)
				{
				case 0:
					type = (Type)atoi(buffer);
					break;
				case 1:
					customName = std::string(buffer);
					break;
				}
				break;
			case Portals:
				switch (li)
				{
				case 0:
					ptx = atoi(buffer);
					break;
				case 1:
					pty = atoi(buffer);
					portal = addPortal(ptx, pty);
					break;
				case 2:
					if(portal) portal->name = std::string(buffer);
					break;
				case 3:
					if(portal) portal->destMapName = std::string(buffer);
					break;
				case 4:
					if(portal) portal->destPortalName = std::string(buffer);
					break;
				case 5:
					if(buffer[0] == '\0')
					{
						li = -1;
						break;
					}
					if(portal) portal->outDirAngleDeg = atoi(buffer);
					break;
				case 6:
					if(portal) portal->outWalkAmount = atoi(buffer);
					break;
				case 7:
					li = -1;
					break;
				}
				break;
			case CollLines:
				switch (li)
				{
				case 0:
					clp1.x = atof(buffer);
					break;
				case 1:
					clp1.y = atof(buffer);
					break;
				case 2:
					clp2.x = atof(buffer);
					break;
				case 3:
					clp2.y = atof(buffer);
					break;
				case 4:
					collLines.push_back(new CollisionLine(clp1.x, clp1.y, clp2.x, clp2.y));
					li = -1;
					break;
				}
				break;
			case Roads:
				if (li == 0 && buffer[0] == '\0')
				{
					li = -1;
					break;
				}
				if (li == 0)
				{
					road = new Road();
					addEnt(road, false);
					road->width = atof(buffer);
				}
				else if (li == 1)
				{
					if (road) road->laneCount = atoi(buffer);
				}
				else if (li >= 2)
				{
					if (road)
					{
						char* token = strtok(buffer, ",");
						while (token)
						{
							int x = atoi(token);
							token = strtok(nullptr, ",");
							if (token)
							{
								int y = atoi(token);
								road->anchors.push_back({ x, y });
								token = strtok(nullptr, ",");
							}
						}
					}
					li = -1;
				}
				break;
			default:

				break;
			}
			++li;
		}
		fclose(f);
	}
	else printf("Couldn't read map conf\n");

	FILE* lightsFile = fopen(lightsConfLoc.c_str(), "r");
	if (lightsFile)
	{
		char buffer[8192];
		Light* light = nullptr;
		int li = 0, sl;
		while (fgets(buffer, sizeof(buffer), lightsFile) != NULL)
		{
			sl = strlen(buffer);
			if (sl > 0) sl--;
			buffer[sl] = '\0'; //replace \n with \0
			
			if (li == 0 && buffer[0] == '\0')
			{
				continue;
			}
			if (li == 0)
			{
				light = new Light();
				addEnt(light, false);
				light->posX = atof(buffer);
			}
			else if (li == 1)
			{
				if (light) light->posY = atof(buffer);
			}
			else if (li == 2)
			{
				if (light) light->intensity = atof(buffer);
			}
			else if (li == 3)
			{
				if (light)
				{
					int r, g, b;
					sscanf(buffer, "%d,%d,%d", &r, &g, &b);
					light->colorR = r;
					light->colorG = g;
					light->colorB = b;
				}
				li = -1;
			}
			++li;
		}
		fclose(lightsFile);
	}

	FILE* buildingsFile = fopen(buildingsConfLoc.c_str(), "r");
	if (buildingsFile)
	{
		char buffer[8192];
		Building* curBuilding = nullptr;
		Floor* curFloor = nullptr;
		int li = 0; // line index within current building header

		while (fgets(buffer, sizeof(buffer), buildingsFile) != NULL)
		{
			int sl = strlen(buffer);
			while (sl > 0 && (buffer[sl - 1] == '\n' || buffer[sl - 1] == '\r')) buffer[--sl] = '\0';

			if (buffer[0] == '\0') continue;

			if (buffer[0] == '[') {
				curBuilding = new Building();
				// Building constructor adds one floor by default, but we will load floors from file
				for (auto f : curBuilding->floors) delete f;
				curBuilding->floors.clear();

				buildings.push_back(curBuilding);
				curBuilding->name = std::string(buffer + 1, sl - 2);
				curFloor = nullptr;
				li = 0;
				continue;
			}

			if (curBuilding) {
				if (li == 0) {
					sscanf(buffer, "%lf,%lf", &curBuilding->posX, &curBuilding->posY);
					li++;
				}
				else {
					if (buffer[0] == 'f') {
						curFloor = new Floor();
						curBuilding->floors.push_back(curFloor);
						sscanf(buffer, "f %f", &curFloor->height);
					}
					else if (curFloor) {
						double x1, y1, x2, y2;
						int tid;
						if (sscanf(buffer, "%lf,%lf,%lf,%lf,%d", &x1, &y1, &x2, &y2, &tid) == 5) {
							curFloor->walls.push_back(BuildingWall(x1, y1, x2, y2, tid));
						}
					}
				}
			}
		}
		fclose(buildingsFile);
		
		// Spawn physics-enabled wall entities for all loaded buildings
		for (Building* b : buildings) {
			//if (!disableBuildingWallPhysics)
			{
				b->spawnWallEntities(this);
			}
		}
	}
}

void Map::save()
{
	//TODO: add backups for existing map config
	
	for (auto e : ents_to_add)
	{
		entities.push_back(e);
	}
	ents_to_add.clear();

	FILE* f = fopen(mapConfLoc.c_str(), "w");
	if (f)
	{
		fprintf(f, "%d\n", type);
		fprintf(f, "%s\n", customName.c_str());

		size_t i = 0;

		fprintf(f, "@\n");
		for(auto& p : portals)
		{
			fprintf(f, "%d\n", p.tilePosX);
			fprintf(f, "%d\n", p.tilePosY);
			fprintf(f, "%s\n", p.name.c_str());
			fprintf(f, "%s\n", p.destMapName.c_str());
			fprintf(f, "%s\n", p.destPortalName.c_str());
			fprintf(f, "%d\n", p.outDirAngleDeg);
			fprintf(f, "%d\n", p.outWalkAmount);
			fprintf(f, "\n");
			++i;
		}

		i = 0;
		fprintf(f, "@\n");
		for(CollisionLine* cl : collLines)
		{
			fprintf(f, "%f\n", cl->p1.x);
			fprintf(f, "%f\n", cl->p1.y);
			fprintf(f, "%f\n", cl->p2.x);
			fprintf(f, "%f\n", cl->p2.y);
			fprintf(f, "\n");
			++i;
		}

		i = 0;
		fprintf(f, "@\n");
		for(Entity* e : entities)
		{
			if (Road* road = dynamic_cast<Road*>(e))
			{
				if (road->flags & Entity::EntFlag_MissionSpawned) continue;
				fprintf(f, "%f\n", road->width);
				fprintf(f, "%d\n", road->laneCount);
				for (size_t j = 0; j < road->anchors.size(); ++j)
				{
					if (j > 0) fprintf(f, ",");
					fprintf(f, "%d,%d", road->anchors[j].x, road->anchors[j].y);
				}
				fprintf(f, "\n\n");
				++i;
			}
		}

		fclose(f);
		
		//printf("Saved map to disk\n");
		
		FILE* f = fopen(lightsConfLoc.c_str(), "w");
		if (f)
		{
			int lightsWritten = 0;
			for(Entity* e : entities)
			{
				if (Light* light = dynamic_cast<Light*>(e))
				{
					if (light->flags & Entity::EntFlag_MissionSpawned) continue;
					fprintf(f, "%f\n", light->posX);
					fprintf(f, "%f\n", light->posY);
					fprintf(f, "%f\n", light->intensity);
					fprintf(f, "%d,%d,%d\n\n", light->colorR, light->colorG, light->colorB);
					++lightsWritten;
				}
			}
			fclose(f);
		}

		f = fopen(buildingsConfLoc.c_str(), "w");
		if (f)
		{
			for (auto b : buildings)
			{
				fprintf(f, "[%s]\n", b->name.c_str());
				fprintf(f, "%f,%f\n", b->posX, b->posY);
				for (auto fl : b->floors) {
					fprintf(f, "f %f\n", fl->height);
					for (auto& w : fl->walls) {
						fprintf(f, "%f,%f,%f,%f,%d\n", w.x1, w.y1, w.x2, w.y2, w.textureId);
					}
				}
				fprintf(f, "\n");
			}
			fclose(f);
		}
	}
	else printf("Couldn't save map\n");
}

Map::~Map()
{
	std::cout << "Destroying map \"" << name << "\" [" << customName << "]" << std::endl;
	delete worldCrime;

	for(Entity* e : entities)
	{
		delete e;
	}
	for(Entity* e : ents_to_add)
	{
		delete e;
	}
	for(CollisionLine* cl : collLines)
	{
		delete cl;
	}
	for (auto b : buildings)
	{
		delete b;
	}
	buildings.clear();
	unloadAllChunks();
}

bool compareEntY(Entity* e1, Entity* e2)
{
	return e1->posY < e2->posY;
}

void Map::update() 
{
	auto& igs = IGS;
	for (auto e : ents_to_add)
	{
		entities.push_back(e);
	}
	ents_to_add.clear();
	recalcPathfindingNodes();
	
	worldCrime->update();

	chunks.erase(
		std::remove_if(chunks.begin(), chunks.end(), [](const MapChunk* c)
		{
			return !c->shouldKeepLoaded();
		}), 
		chunks.end()
	);

	entities.erase(std::remove_if(entities.begin(), entities.end(), [](Entity* e) -> bool
	{
		if (e->flags & Entity::EntFlag_Dead)
		{
			//assert(e != IGS->player);
			return true;
		}
		return false;
	}), entities.end());

#if DEBUG
	sf::CircleShape anchorCs{2};
	anchorCs.setOrigin(4, 4);
	anchorCs.setFillColor({255, 0, 0, 255 / 4});
	anchorCs.setOutlineColor({255, 255, 255});
	anchorCs.setOutlineThickness(1);

	if (IGS->collisionDebug)
	{
		int i = 0;
		auto it = collLines.begin();
		while (it != collLines.end())
		{
			auto& cl = *it;

			bool delLine = false;

			if (IGS->newCollLineState == InGameScreen::DrawNewCollLineState_Done)
			{
				anchorCs.setPosition(sf::Vector2f{ (float)cl->p1.x, (float)cl->p1.y });
				if (anchorCs.getGlobalBounds().contains({ (float)IGS->mousePosOnMapX, (float)IGS->mousePosOnMapY }))
				{
					if (Mouse::LeftFrames == 1)
					{
						IGS->grabbedCollAnchor = true;
						Mouse::LeftFrames = 0;
						IGS->movingV2fps.push_back(&cl->p1);
					}
					if (Mouse::RightRel)
					{
						delLine = true;
					}
				}

				anchorCs.setPosition(sf::Vector2f{ (float)cl->p2.x, (float)cl->p2.y });
				if (anchorCs.getGlobalBounds().contains({ (float)IGS->mousePosOnMapX, (float)IGS->mousePosOnMapY }))
				{
					if (Mouse::LeftFrames == 1)
					{
						IGS->grabbedCollAnchor = true;
						Mouse::LeftFrames = 0;
						IGS->movingV2fps.push_back(&cl->p2);
					}
					if (Mouse::RightRel)
					{
						delLine = true;
					}
				}
			}

			if (delLine)
			{
				it = collLines.erase(it);
				Sfx::Close->play();
			}
			else
			{
				++i;
				++it;
			}
		}
	}
	if (IGS->roadEditOn)
	{
		if (IGS->grabbedRoadAnchorRoad && IGS->grabbedRoadAnchorIdx != -1)
		{
			auto& anchor = IGS->grabbedRoadAnchorRoad->anchors[IGS->grabbedRoadAnchorIdx];
			anchor.x = (int)IGS->mouseTilePosMidClosestX;
			anchor.y = (int)IGS->mouseTilePosMidClosestY;
		}
		else if (!IGS->grabbedCollAnchor)
		{
			for (Entity* e : entities)
			{
				if (Road* road = dynamic_cast<Road*>(e))
				{
					for (size_t a = 0; a < road->anchors.size(); ++a)
					{
						auto& anchor = road->anchors[a];
						sf::Vector2f anchorWorldPos(anchor.x * TileSizePixels, anchor.y * TileSizePixels);
						if (Util::Dist(IGS->mousePosOnMapX, IGS->mousePosOnMapY, anchorWorldPos.x, anchorWorldPos.y) <= 16.0f)
						{
							if (Mouse::LeftFrames == 1)
							{
								IGS->grabbedCollAnchor = true;
								IGS->grabbedRoadAnchorRoad = road;
								IGS->grabbedRoadAnchorIdx = (int)a;
								IGS->lastEditedRoad = road;
							}
						}
					}
				}
			}
		}
	}

	if (IGS->newBuildingWallState != InGameScreen::DrawNewBuildingWallState_None)
	{
		if (IGS->newBuildingWallState == InGameScreen::DrawNewBuildingWallState_FirstPoint)
		{
			if (Mouse::LeftFrames == 1)
			{
				if (buildings.empty())
				{
					auto b = new Building();
					b->name = "New Building";
					b->posX = IGS->mousePosOnMapX;
					b->posY = IGS->mousePosOnMapY;
					buildings.push_back(b);
					IGS->lastEditedBuilding = b;
					IGS->lastEditedFloor = b->floors[0];
				}
				else if (!IGS->lastEditedBuilding)
				{
					IGS->lastEditedBuilding = buildings[0];
					IGS->lastEditedFloor = buildings[0]->floors[0];
				}

				auto fl = IGS->lastEditedFloor;
				double rx = IGS->mousePosOnMapX - IGS->lastEditedBuilding->posX;
				double ry = IGS->mousePosOnMapY - IGS->lastEditedBuilding->posY;
				fl->walls.push_back(BuildingWall(rx, ry, rx, ry));
				IGS->grabbedBuildingWall = &fl->walls.back();
				IGS->grabbedBuildingWallAnchorIdx = 1;
				IGS->newBuildingWallState = InGameScreen::DrawNewBuildingWallState_SecondPoint;
				Mouse::LeftFrames = 0;
			}
		}
		else if (IGS->newBuildingWallState == InGameScreen::DrawNewBuildingWallState_SecondPoint)
		{
			if (IGS->grabbedBuildingWall == nullptr)
			{
				IGS->newBuildingWallState = InGameScreen::DrawNewBuildingWallState_None;
			}
		}
	}

	if (IGS->buildingEditOn)
	{
		if (IGS->grabbedBuildingWall && IGS->grabbedBuildingWallAnchorIdx != -1 && IGS->lastEditedBuilding)
		{
			if (IGS->grabbedBuildingWallAnchorIdx == 0)
			{
				IGS->grabbedBuildingWall->x1 = IGS->mousePosOnMapX - IGS->lastEditedBuilding->posX;
				IGS->grabbedBuildingWall->y1 = IGS->mousePosOnMapY - IGS->lastEditedBuilding->posY;
			}
			else if (IGS->grabbedBuildingWallAnchorIdx == 1)
			{
				IGS->grabbedBuildingWall->x2 = IGS->mousePosOnMapX - IGS->lastEditedBuilding->posX;
				IGS->grabbedBuildingWall->y2 = IGS->mousePosOnMapY - IGS->lastEditedBuilding->posY;
			}
			else if (IGS->grabbedBuildingWallAnchorIdx == 2)
			{
				// Dragging the whole wall
				IGS->grabbedBuildingWall->x1 = (IGS->mousePosOnMapX - IGS->lastEditedBuilding->posX) + IGS->grabbedBuildingWallOffset1X;
				IGS->grabbedBuildingWall->y1 = (IGS->mousePosOnMapY - IGS->lastEditedBuilding->posY) + IGS->grabbedBuildingWallOffset1Y;
				IGS->grabbedBuildingWall->x2 = (IGS->mousePosOnMapX - IGS->lastEditedBuilding->posX) + IGS->grabbedBuildingWallOffset2X;
				IGS->grabbedBuildingWall->y2 = (IGS->mousePosOnMapY - IGS->lastEditedBuilding->posY) + IGS->grabbedBuildingWallOffset2Y;
			}

			if (Mouse::LeftRel) // Left click released
			{
				IGS->lastEditedBuilding->spawnWallEntities(this);
				IGS->grabbedBuildingWall = nullptr;
				IGS->grabbedBuildingWallAnchorIdx = -1;
			}
		}
		else if (!IGS->grabbedCollAnchor)
		{
			IGS->hoveredBuildingWall = nullptr;
			IGS->hoveredBuildingWallAnchorIdx = -1;

			for (Building* b : buildings)
			{
				for (Floor* fl : b->floors)
				{
					auto it = fl->walls.begin();
					while (it != fl->walls.end())
					{
						auto& w = *it;
						bool delWall = false;

						// Check p1 (x1, y1)
						anchorCs.setPosition(sf::Vector2f{ (float)w.x1 + (float)b->posX, (float)w.y1 + (float)b->posY });
						if (anchorCs.getGlobalBounds().contains({ (float)IGS->mousePosOnMapX, (float)IGS->mousePosOnMapY }))
						{
							IGS->hoveredBuildingWall = &w;
							IGS->hoveredBuildingWallAnchorIdx = 0;

							if (Mouse::LeftFrames == 1)
							{
								IGS->grabbedBuildingWall = &w;
								IGS->grabbedBuildingWallAnchorIdx = 0;
								IGS->lastEditedBuilding = b;
								IGS->lastEditedFloor = fl;
							}
							if (Mouse::RightRel)
							{
								delWall = true;
							}
						}

						// Check p2 (x2, y2)
						if (!IGS->grabbedBuildingWall && !IGS->hoveredBuildingWall) // Only check second point if first wasn't hovered/grabbed
						{
							anchorCs.setPosition(sf::Vector2f{ (float)w.x2 + (float)b->posX, (float)w.y2 + (float)b->posY });
							if (anchorCs.getGlobalBounds().contains({ (float)IGS->mousePosOnMapX, (float)IGS->mousePosOnMapY }))
							{
								IGS->hoveredBuildingWall = &w;
								IGS->hoveredBuildingWallAnchorIdx = 1;

								if (Mouse::LeftFrames == 1)
								{
									IGS->grabbedBuildingWall = &w;
									IGS->grabbedBuildingWallAnchorIdx = 1;
									IGS->lastEditedBuilding = b;
									IGS->lastEditedFloor = fl;
								}
								if (Mouse::RightRel)
								{
									delWall = true;
								}
							}
						}

						// Check clicking on the wall segment itself
						if (!IGS->grabbedBuildingWall && !IGS->hoveredBuildingWall)
						{
							double wx1 = w.x1 + b->posX;
							double wy1 = w.y1 + b->posY;
							double wx2 = w.x2 + b->posX;
							double wy2 = w.y2 + b->posY;
							
							// Distance to segment check
							double dist = Util::DistToLine(IGS->mousePosOnMapX, IGS->mousePosOnMapY, wx1, wy1, wx2, wy2);
							
							// Simple bounding box check with padding
							double minX = std::min(wx1, wx2) - 4.0;
							double maxX = std::max(wx1, wx2) + 4.0;
							double minY = std::min(wy1, wy2) - 4.0;
							double maxY = std::max(wy1, wy2) + 4.0;
							
							if (dist < 4.0 && IGS->mousePosOnMapX >= minX && IGS->mousePosOnMapX <= maxX && 
								IGS->mousePosOnMapY >= minY && IGS->mousePosOnMapY <= maxY)
							{
								IGS->hoveredBuildingWall = &w;
								IGS->hoveredBuildingWallAnchorIdx = 2; // Whole wall

								if (Mouse::LeftFrames == 1)
								{
									IGS->grabbedBuildingWall = &w;
									IGS->grabbedBuildingWallAnchorIdx = 2; // Whole wall
									IGS->lastEditedBuilding = b;
									IGS->lastEditedFloor = fl;
									
									// Store offsets from mouse to endpoints
									IGS->grabbedBuildingWallOffset1X = w.x1 - (IGS->mousePosOnMapX - b->posX);
									IGS->grabbedBuildingWallOffset1Y = w.y1 - (IGS->mousePosOnMapY - b->posY);
									IGS->grabbedBuildingWallOffset2X = w.x2 - (IGS->mousePosOnMapX - b->posX);
									IGS->grabbedBuildingWallOffset2Y = w.y2 - (IGS->mousePosOnMapY - b->posY);
								}
							}
						}

						if (delWall)
						{
							it = fl->walls.erase(it);
							b->spawnWallEntities(this);
							Sfx::Close->play();
						}
						else
						{
							++it;
						}
					}
				}
			}
		}
	}
#endif
	//sorted in the renderer
	//std::sort(entities.begin(), entities.end(), compareEntY);

	for (auto e : entities)
	{
		e->updateTPos();
		e->update();
		if (IGS->curMission)
		{
			IGS->curMission->onEntityTick(e);
		}
	}

	for (size_t i = 0; i < entities.size(); ++i)
	{
		for (size_t j = i + 1; j < entities.size(); ++j)
		{
			if(!(entities[j]->flags & Entity::EntFlag_UnderAllOtherEnts)) entities[i]->resolveCollisionWith(entities[j]);
		}
	}

	for (auto& p : portals) 
	{
		sf::Vector2f ppos((p.tilePosX * TileSizePixels) + (TileSizePixels / 2),
			(p.tilePosY * TileSizePixels) + (TileSizePixels / 2));
		for (auto e : entities)
		{
			//if (!e.get())continue;//wtf
#if DEBUG
			if (e == IGS->player && Util::Dist(e->posX, e->posY, ppos.x, ppos.y) < (float)TileSizePixels)
			{
				IGS->interactables.push_back(IGS->deletePortalInter);
				IGS->interactables.push_back(IGS->editPortalInter);
				IGS->portalNearPlayer = &p;
			}
#endif
			if (sf::Vector2i{ p.tilePosX, p.tilePosY } == sf::Vector2i{ e->tilePosX, e->tilePosY } && ((e->flags & Entity::EntFlag_Animating) || (e != igs->player && e->physicsSlideAmount != 0)))
			{
				if (e == igs->player)
				{
					if ((e->flags & Entity::EntFlag_TPosChanged) && IGS->mapFactory->find(p.destMapName))
					{
						igs->mcPortalUsed = &p;
						igs->switchMaps(p.destMapName);
						auto act = (Actor*)e;
						act->isControllable = false;
						act->flags &= ~Entity::EntFlag_Animating;
						IGS->playerAutoWalking = false; // <-- Add this line to fix softlock
					}
				}
				else
				{
					if(!e->portalActivateCooldown->once(5000)) continue;
					e->physicsSlideAmount = p.outWalkAmount;
					e->physicsSlideAngle = p.outDirAngleDeg;
					p.activate(e);
				}
			}
		}
	}
}

bool Map::canAddToPathNodeList(double pX, double pY, CollisionLine* ignore)
{
	bool good = true;
	//check if too close to collision line
	double thresh = 6.0;
	for(CollisionLine* cl : collLines)
	{
		if (cl == ignore)
		{
			continue;
		}
		if (Util::Dist(pX, pY, (double)cl->p1.x, (double)cl->p1.y) < thresh || Util::Dist(pX, pY, (double)cl->p2.x, (double)cl->p2.y) < thresh)
		{
			good = false;
			break;
		}
	}
	return good;
}

bool Map::doesLineIntersectCollisionLine(double p1X, double p1Y, double p2X, double p2Y)
{
	for(CollisionLine* cl : collLines)
	{
		if (Util::Intersects(p1X, p1Y, p2X, p2Y, (double)cl->p1.x, (double)cl->p1.y, (double)cl->p2.x, (double)cl->p2.y))
		{
			return true;
		}
	}
	return false;
}

void Map::recalcPathfindingNodes()
{
	pathFindingNodePoints.clear();
	for(CollisionLine* cl : collLines)
	{
		double cx = (double)cl->p1.x - (double)cl->p2.x;
		double cy = (double)cl->p1.y - (double)cl->p2.y;
		double lineLen = Util::Clamp(sqrt(cx * cx + cy * cy), 0.0, 2000.0);

		double outDist = 8.0;
		double collLineAngle = Util::RotateTowards((double)cl->p1.x, (double)cl->p1.y, (double)cl->p2.x, (double)cl->p2.y);
		double collLineAngleR = Util::RotateTowards((double)cl->p2.x, (double)cl->p2.y, (double)cl->p1.x, (double)cl->p1.y);
		double leftA = fmod(collLineAngle - 90.0, 360.0);
		double rightA = fmod(collLineAngle + 90.0, 360.0);

		Point behindPoint = {};
		Util::AngleLineRel((double)cl->p1.x, (double)cl->p1.y, collLineAngleR, outDist, &behindPoint.x, &behindPoint.y);
		Point frontPoint = {};
		Util::AngleLineRel((double)cl->p2.x, (double)cl->p2.y, collLineAngle, outDist, &frontPoint.x, &frontPoint.y);

		auto addOffsetPoints = [&](const Point& point, double angle) {
			double sideOffset = 8.0; // Distance to offset points to the sides
			double sideAngle1 = fmod(angle + 90.0, 360.0); // 90 degrees to the right
			double sideAngle2 = fmod(angle - 90.0, 360.0); // 90 degrees to the left
			
			Point rightPoint = {};
			Util::AngleLineRel(point.x, point.y, sideAngle1, sideOffset, &rightPoint.x, &rightPoint.y);
			Point leftPoint = {};
			Util::AngleLineRel(point.x, point.y, sideAngle2, sideOffset, &leftPoint.x, &leftPoint.y);
			
			if (canAddToPathNodeList(rightPoint.x, rightPoint.y, cl)) pathFindingNodePoints.push_back({ rightPoint.x, rightPoint.y });
			if (canAddToPathNodeList(leftPoint.x, leftPoint.y, cl)) pathFindingNodePoints.push_back({ leftPoint.x, leftPoint.y });
		};

		if (canAddToPathNodeList(frontPoint.x, frontPoint.y, cl)) {
			pathFindingNodePoints.push_back({ frontPoint.x, frontPoint.y });
			addOffsetPoints(frontPoint, collLineAngle);
		}
		if (canAddToPathNodeList(behindPoint.x, behindPoint.y, cl)) {
			pathFindingNodePoints.push_back({ behindPoint.x, behindPoint.y });
			addOffsetPoints(behindPoint, collLineAngleR);
		}

		int stopCount = (int)(lineLen / 12.0);
		if (stopCount > 0) {
			double stopDiv = lineLen / (double)stopCount;
			for (int i = 0; i < stopCount; ++i)
			{
				Point stop = {};
				Util::AngleLineRel((double)cl->p1.x, (double)cl->p1.y, collLineAngle, (stopDiv * (double)(i + 1)) - (stopDiv / 2.0), &stop.x, &stop.y);
				Point stopL = {};
				Util::AngleLineRel(stop.x, stop.y, leftA, outDist, &stopL.x, &stopL.y);
				Point stopR = {};
				Util::AngleLineRel(stop.x, stop.y, rightA, outDist, &stopR.x, &stopR.y);
				if (canAddToPathNodeList(stopL.x, stopL.y, cl)) pathFindingNodePoints.push_back({ stopL.x, stopL.y });
				if (canAddToPathNodeList(stopR.x, stopR.y, cl)) pathFindingNodePoints.push_back({ stopR.x, stopR.y });
			}
		}
	}
	needPathfindingNodesRecalc = false;
}

bool Map::removeEnt(Entity* e) 
{
	auto it = std::find(entities.begin(), entities.end(), e);
	if (it != entities.end())
	{
		entities.erase(it);
		return true;
	}

	it = std::find(ents_to_add.begin(), ents_to_add.end(), e);
	if (it != ents_to_add.end())
	{
		ents_to_add.erase(it);
		return true;
	}

	return false;
}

Portal* Map::getPortal(std::string name)
{
	for(auto& p : portals)
	{
		if (p.name == name)
		{
			return &p;
		}
	}
	return nullptr;
}

Portal* Map::addPortal(int tx, int ty)
{
	portals.push_back(Portal(tx, ty));
	auto& np = portals.back();
	np.hostMap = this;
	return &np;
}

void Map::delPortal(Portal* port) 
{
	portals.erase(std::remove_if(portals.begin(), portals.end(), [&](Portal& p) -> bool
	{
		bool r = &p == port;
		return r;
	}));
}

MapChunk* Map::addChunk(int x, int y) 
{
	chunks.push_back(new MapChunk(this, x, y));
	return chunks.back();
}

void Map::unloadAllChunks()
{
	//std::printf("unloaded all chunks\n");
	for(auto c : chunks)
	{
		delete c;
	}
	chunks.clear();
}

void Map::getTILE(int x, int y, int layer, MapTile** out)
{
	int cx = (x >= 0) ? (x / MapChunk::SizeTiles) : ((x + 1) / MapChunk::SizeTiles - 1);
	int cy = (y >= 0) ? (y / MapChunk::SizeTiles) : ((y + 1) / MapChunk::SizeTiles - 1);
	auto ch = getChunk(cx, cy);
	if (ch && out)
	{
		int lx = x % MapChunk::SizeTiles; if (lx < 0) lx += MapChunk::SizeTiles;
		int ly = y % MapChunk::SizeTiles; if (ly < 0) ly += MapChunk::SizeTiles;
		*out = &(MapTile&)ch->getTILE(lx, ly, layer);
	}
}

MapChunk* Map::getChunk(int x, int y, bool create) 
{
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		auto& c=**it;
		if (c.x == x && c.y == y) {
			c.keepAlive();
			return &c;
		}
	}
	if (create) {
		return addChunk(x, y);
	}
	return nullptr;
}

void Map::moveEntHere(Entity* e, int posX, int posY)
{
	if (e->hostMap != this)
	{
		e->hostMap->removeEnt(e);
		addEnt(e, e->flags & Entity::EntFlag_MissionSpawned);
		if (auto a = dynamic_cast<Actor*>(e))
		{
			a->astn = nullptr;
		}
	}

	e->setPosTile(posX, posY);
	lastPortalSpawnInTileX = posX;
	lastPortalSpawnInTileY = posY;
}

std::vector<Entity*> Map::getMissionSpawnedEnts() 
{
	std::vector<Entity*> ret;
	for (auto e : entities)
	{
		if (e->flags & Entity::EntFlag_MissionSpawned)
		{
			ret.push_back(e);
		}
	}
	return ret;
}

void Map::addEnt(Entity* e, bool missionSpawned)
{
#if DEBUG
    bool found = false;
    for (auto existingEnt : entities)
    {
        if (existingEnt == e) {
            found = true;
            break;
        }
    }
    if (found) {
        std::cerr << "Entity already exists in Map::addEnt\n" << std::endl;
        assert(!found && "Entity is already in entities or ents_to_add list");
    }
#endif
	e->hostMap = this;
	if (missionSpawned)
	{
		e->flags |= Entity::EntFlag_MissionSpawned;
	}
	if (G->framesPassed == 0) 
	{
		entities.push_back(e);
	}
	else 
	{
		ents_to_add.push_back(e);
	}
}

}