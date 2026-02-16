#include "Map/Map.h"
#include "Game/Util.h"
#include "Entity/Entity.h"
#include "Texture/Sheet.h"
#include "Game/Game.h"
#include <cassert>
#include <iostream>

namespace nyaa {

std::string MapChunk::getChunkFilePath() 
{
	return Util::Format("%schunk/%d,%d", host->folderLoc.c_str(), x, y);
}

MapChunk::MapChunk(Map* host, int x, int y)
	:modified(false)
	,x(x)
	,y(y)
	,host(host)
{
	auto path = getChunkFilePath();
	FILE* cf = fopen(path.c_str(), "r");
	if (cf) {
		// std::printf("loaded chunk %d,%d from disk\n", this->x, this->y);
		for (int li = 0; li < LayerCount; ++li)
		{
			for (unsigned int ty = 0; ty < SizeTiles; ++ty)
			{
				for (unsigned int tx = 0; tx < SizeTiles; ++tx)
				{
					fscanf(cf, "%hu\n", &getTILE(tx, ty, li).id);
				}
			}
		}
		fclose(cf);
	}
}

void MapChunk::save(std::string folderLoc) 
{
	if (!modified)
	{
		return;
	}
	assert(folderLoc.size() >= 1);
	if (folderLoc.size() < 1) return;
	char folderLocEndChar = folderLoc[folderLoc.size() - 1];
	assert(folderLocEndChar == '\\' || folderLocEndChar == '/');

	auto chunksFolder = Util::Format("%schunk/", host->folderLoc.c_str());
	Util::MakeDir(chunksFolder);
	auto chunkFileLoc = getChunkFilePath();

	FILE* cf = fopen(chunkFileLoc.c_str(), "w");
	if (cf) {
		for (int li = 0; li < LayerCount; ++li) 
		{
			for (unsigned int ty = 0; ty < SizeTiles; ++ty) 
			{
				for (unsigned int tx = 0; tx < SizeTiles; ++tx) 
				{
					fprintf(cf, "%u\n", getTILE(tx, ty, li).id);
				}
			}
		}

		// #if DEBUG
		// 		printf("saved chunk x%d y%d to %s\n", x, y, chunkFileLoc);
		// #endif

		fclose(cf);
	}
}

MapChunk::~MapChunk() 
{
	std::cout << "{" << G->framesPassed << "} Destroying chunk [" << x << "x, " << y << "y]" << std::endl;
	//std::cout << "Deleted chunk" << std::endl;
#if DEBUG
	save(host->folderLoc);
#endif
}

void MapChunk::GetPosInWorld(int x, int y, float* outX, float* outY)
{
	if (outX) *outX = x * SizePixels;
	if (outY) *outY = y * SizePixels;
}

//sf::Vector2f MapChunk::getCenter()
//{
//	return getCenter(x, y);
//}
//sf::Vector2f MapChunk::getCenter(int x, int y)
//{
//	return { (float)((x * SizePixels) + (SizePixels / 2)), (float)((y * SizePixels) + (SizePixels / 2)) };
//}

void MapChunk::keepAlive() 
{
	clock.restart();
}

bool MapChunk::shouldKeepLoaded() const 
{
	return clock.getElapsedSeconds() < (float)StayLoadedSeconds;
}

bool MapChunk::isEntInChunk(Entity* e) 
{
	return e->chunkPosX == x && e->chunkPosY == y;
}

MapTile& MapChunk::getTILE(unsigned int x, unsigned int y, unsigned int layer) 
{
	assert(layer < MapChunk::LayerCount);
	auto& ret = data[layer][y][x];
	return ret;
}

void MapChunk::set(unsigned int x, unsigned int y, unsigned int layer, unsigned int id) 
{
	auto& t = (MapTile&)getTILE(x, y, layer);
	t.id = id;//WRITING
	modified = true;
}

}