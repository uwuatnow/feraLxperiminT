#pragma once
#include <vector>
#include <string>

namespace nyaa {

class CollisionLine;
class Map;

class BuildingWall {
public:
    double x1, y1, x2, y2;
    int textureId = 0;
    bool destructable = true; // Whether this wall can be destroyed
    
    BuildingWall() : x1(0), y1(0), x2(0), y2(0), textureId(0), destructable(true) {}
    BuildingWall(double x1, double y1, double x2, double y2, int textureId = 0) 
        : x1(x1), y1(y1), x2(x2), y2(y2), textureId(textureId), destructable(true) {}
};

class Floor {
public:
    std::vector<BuildingWall> walls;
    float height = 32.0f; // Default floor height
    
    Floor() {}
    ~Floor() {}
};

class Building {
public:
    std::vector<Floor*> floors;
    std::string name;
    double posX = 0, posY = 0;
    bool usePhysicsWalls = true; // Whether to spawn physics-enabled wall entities
    
    Building() {
        // Index 0 is the base of the building
        floors.push_back(new Floor());
    }
    
    ~Building() {
        for (auto floor : floors) {
            delete floor;
        }
        floors.clear();
    }
    
    void addFloor() {
        floors.push_back(new Floor());
    }
    
    void removeFloor(int index) {
        if (index >= 0 && index < (int)floors.size()) {
            delete floors[index];
            floors.erase(floors.begin() + index);
        }
    }
    
    // Spawn physics-enabled wall entities for this building
    void spawnWallEntities(Map* map);
    void clearWallEntities(Map* map);

    std::vector<class BuildingWallEntity*> spawnedEntities;
};

} // namespace nyaa
