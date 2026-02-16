#pragma once
#include "Game/Globals.h"
#include <string>
#include "Game/Clock.h"
#include "Map/MapTile.h"

namespace nyaa {

class Map;
class Entity;

class MapChunk
{
public:
	enum : int 
	{
		SizeTiles = 32, 
		LayerCount = 3, 
		StayLoadedSeconds = 10, 
		SizePixels = /*Map::TileSizePixels*/16 * SizeTiles
	};

public:
	MapChunk(Map* host, int x, int y);
	
	~MapChunk();
	
public:
	std::string getChunkFilePath();
	
	static void GetPosInWorld(int x, int y, float* outX, float* outY);

	void keepAlive();
	
	bool shouldKeepLoaded() const;
	
	bool isEntInChunk(Entity* e);
	
	MapTile& getTILE(unsigned int x, unsigned int y, unsigned int layer);
	
	void set(unsigned int x, unsigned int y, unsigned int layer, unsigned int id);
	
	void save(std::string folderLoc);

public:
	bool modified;
	int x, y; //pos in map
	Map* host;
	Clock clock;
	MapTile data[LayerCount][SizeTiles][SizeTiles]; //[layer][y][x]
};

}
