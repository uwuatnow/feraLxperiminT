#pragma once
#include "Game/Globals.h"

#include <string>
#include <vector>

namespace nyaa {

class MapFactory
{
public:
	MapFactory();
	
	~MapFactory();

public:
	class Map* getCurrentMap();
	
	bool switchTo(std::string name); // returns true if the switch was successful
	
	class Map* find(std::string name);
	
	class Map* get(std::string name);//inits if not found unlike find(...)
	
	class Map* add(class Map* map);

	class Map* find(int idx);
	
	int findIdx(std::string name);
	
	int findIdx(class Map* map);

public:
	std::vector<class Map*> maps;
	size_t cur_map;
};

}