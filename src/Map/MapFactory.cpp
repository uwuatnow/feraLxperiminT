#include "MapFactory.h"
#include "Screen/InGameScreen.h"
#include <cassert>
#include "Map/Map.h"
#include <cassert>

namespace nyaa {

MapFactory::MapFactory() 
	:cur_map(0)
{

}

MapFactory::~MapFactory()
{
	std::cout << "Destroying Map factory" << std::endl;
	for (auto m : maps)
	{
		if (m)
		{
			delete m;
		}
	}
	maps.clear();
}

Map* MapFactory::getCurrentMap()
{
	assert(maps.size() > 0 /* && cur_map >= 0  */&& cur_map < maps.size());
	return maps[cur_map];
}

bool MapFactory::switchTo(std::string name)
{
	size_t i = 0;
	for (auto m : maps)
	{
		if (m->name == name)
		{
			cur_map = i;
			return true;
		}
		++i;
	}
	assert(false);
	return false;
}

Map* MapFactory::find(std::string name) {
	for (auto m : maps)
	{
		if (m->name == name)
		{
			return m;
		}
	}
	//assert(false);
	return nullptr;
}

Map* MapFactory::get(std::string name) {
	auto fm = find(name);
	if (fm)
	{
		return fm;
	}
	Map* newMap = new Map(name, *IGS->sheet);
	add(newMap);
	return newMap;
}

Map* MapFactory::add(Map* map) {
	if (std::find(maps.begin(), maps.end(), map) != maps.end())
	{
		return map;
	}
	map->index = maps.size();
	maps.push_back(map);
	return map;
}

Map* MapFactory::find(int idx) {
	int i = 0;
	for (auto m : maps)
	{
		if (idx == i++)
		{
			return m;
		}
	}
	assert(false);
	return nullptr;
}

int MapFactory::findIdx(std::string name) {
	int idx = 0;
	for (auto m : maps)
	{
		if (m->name == name)
		{
			return idx;
		}
		idx++;
	}
	assert(false);
	return -1;
}

int MapFactory::findIdx(Map* map)
{
	int idx = 0;
	for (auto m : maps)
	{
		if (m == map)
		{
			return idx;
		}
		idx++;
	}
	assert(false);
	return -1;
}

}