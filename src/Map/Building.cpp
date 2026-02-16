#include "Map/Building.h"
#include "Map/Map.h"
#include "Entity/BuildingWallEntity.h"

namespace nyaa {

void Building::spawnWallEntities(Map* map) {
    if (!map || !usePhysicsWalls) return;
    
    clearWallEntities(map);
    float currentHeight = 0.0f;
    
    for (Floor* floor : floors) {
        for (const BuildingWall& wall : floor->walls) {
            // Calculate wall position with building offset
            double x1 = posX + wall.x1;
            double y1 = posY + wall.y1;
            double x2 = posX + wall.x2;
            double y2 = posY + wall.y2;
            
            // Create the physics-enabled wall entity
            BuildingWallEntity* wallEntity = new BuildingWallEntity(
                x1, y1, x2, y2, 
                floor->height, 
                wall.textureId
            );
            
            // Set the wall's Z position (height offset)
            // Position the wall so its bottom is at the floor level
            wallEntity->posZ = currentHeight + floor->height * 0.5f;
            
            // Add to the map as an entity
            map->addEnt(wallEntity, false);
            spawnedEntities.push_back(wallEntity);
        }
        
        currentHeight += floor->height;
    }
}

void Building::clearWallEntities(Map* map) {
    for (auto e : spawnedEntities) {
        map->removeEnt(e);
        delete e;
    }
    spawnedEntities.clear();
}

} // namespace nyaa
