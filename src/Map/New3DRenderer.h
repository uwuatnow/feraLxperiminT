#pragma once
#include "Game/Globals.h"
#include <unordered_map>
#include <memory>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Inventory/Drug/DrugEffectsManager.h"

#include "Map/MapRenderer.h"
#include "Game/Clock.h"
#include "Model/Model.h"
#include "Game/Camera.h"

namespace nyaa {

class New3DMainShader;
class BuildingWallEntity;

class New3DRenderer : public MapRenderer
{
public:
    New3DRenderer();
    ~New3DRenderer();
    
    void render(Map* map, RendTarget& target) override;
    
    // Coordinate conversion methods for 3D perspective
    void screenToWorld(int screenPosX, int screenPosY, RendTarget& target, double* outX, double* outY);
    void worldToScreen(double worldPosX, double worldPosY, RendTarget& target, int* outX, int* outY);
    
    // Mouse raycasting for normal vectors
    struct WallHitInfo {
        bool hit;
        double hitX, hitY, hitZ;
        double normalX, normalY, normalZ;
        BuildingWallEntity* wallEntity;
        
        WallHitInfo() : hit(false), hitX(0), hitY(0), hitZ(0), normalX(0), normalY(0), normalZ(0), wallEntity(nullptr) {}
    };
    
public:
    Camera m_camera;
    
private:
    Clock particleTimer;
    float particleOpacity;
    
    Clock shaderClock;
    unsigned int whiteTexture;
    
public:
    New3DMainShader* m_new3DMainShader;
    
private:
    // Debug text rendering system
    struct DebugTextTexture {
        sf::RenderTexture texture;
        sf::Sprite sprite;
        std::string currentText;
        bool isValid;
        
        DebugTextTexture() : isValid(false) {}
    };
    
    std::unordered_map<class Actor*, std::unique_ptr<DebugTextTexture>> debugTextCache;
    
    sf::RenderTexture* getOrCreateTextTexture(class Actor* actor, const std::string& text);
    void updateTextTexture(DebugTextTexture& textTex, const std::string& text);
    
    // Rendering helper functions
    void setupCamera(RendTarget& target);
    void renderFloorTiles(class Map* map, RendTarget& target);
    void renderPortals(class Map* map);
    void renderObjects(class Map* map, RendTarget& target);
    void renderCeilingTiles(class Map* map, RendTarget& target);
    void renderCollisionDebug(class Map* map, RendTarget& target);
    void renderVelocityDebug(class Map* map, RendTarget& target);
    void renderTileEditHighlights(class Map* map, RendTarget& target);
    void renderAutoWalkParticles();
    void renderUrinePuddles(class Map* map) const;
    void renderRoads(class Map* map, RendTarget& target);
    void renderActorShadows(class Map* map, RendTarget& target);
    void renderBuildings(class Map* map, RendTarget& target);
    void renderRain3D();
    void updateLighting(class Map* map);
    
    // Mouse raycasting and normal vector rendering
    void updateMouseRaycast(class Map* map, RendTarget& target);
    void renderNormalVectorArrow(class Map* map, RendTarget& target);
    bool raycastToWall(class Map* map, double rayOriginX, double rayOriginY, double rayOriginZ, 
                       double rayDirX, double rayDirY, double rayDirZ, WallHitInfo& hitInfo);
    
    // Debug rendering functions
    void renderDebugTextAboveActors(class Map* map, RendTarget& target);
    void renderEntityOBBDebug(class Map* map, RendTarget& target);
    
    const float tileSize = 1.0f;

private:
    // Drug effects management - these are now private and controlled internally
    DrugEffectsManager* wavyWorldManager;
    DrugEffectsManager* worldVibrationManager;
    DrugEffectsManager* colorShiftManager;
    DrugEffectsManager* kaleidoscopeManager;
    DrugEffectsManager* noiseManager;
    DrugEffectsManager* greyscaleManager;
    DrugEffectsManager* stretchManager;
    DrugEffectsManager* bitcrushManager;
    DrugEffectsManager* worldScramblerManager;
    DrugEffectsManager* fractalManager;
    DrugEffectsManager* depthPerceptionManager;
	DrugEffectsManager* dissolveManager;
	DrugEffectsManager* rgbDeconvergenceManager;
	DrugEffectsManager* radialBlurManager;
	DrugEffectsManager* bloomManager;
	DrugEffectsManager* columnSplitManager;

public:
    // Public methods to add drug effects (replaces direct float access)
    void addWavyWorldEffect(float effectValue, float durationSeconds);
    void addWorldVibrationEffect(float effectValue, float durationSeconds);
    void addColorShiftEffect(float effectValue, float durationSeconds);
    void addKaleidoscopeEffect(float effectValue, float durationSeconds);
    void addNoiseEffect(float effectValue, float durationSeconds);
    void addGreyscaleEffect(float effectValue, float durationSeconds);
    void addStretchEffect(float effectValue, float durationSeconds);
    void addBitcrushEffect(float effectValue, float durationSeconds);
    void addWorldScramblerEffect(float effectValue, float durationSeconds);
    void addFractalEffect(float effectValue, float durationSeconds);
    void addDepthPerceptionEffect(float effectValue, float durationSeconds);
    void addDissolveEffect(float effectValue, float durationSeconds);
	void addRGBDeconvergenceEffect(float effectValue, float durationSeconds);
	void addRadialBlurEffect(float effectValue, float durationSeconds);
	void addBloomEffect(float effectValue, float durationSeconds);
	void addColumnSplitEffect(float effectValue, float durationSeconds);

	// Public getter methods for accessing current effect values
    float getCurrentWavyWorldEffect() const;
    float getCurrentWorldVibrationEffect() const;
    float getCurrentColorShiftEffect() const;
    float getCurrentKaleidoscopeEffect() const;
    float getCurrentNoiseEffect() const;
    float getCurrentGreyscaleEffect() const;
    float getCurrentStretchEffect() const;
    float getCurrentBitcrushEffect() const;
    float getCurrentWorldScramblerEffect() const;
    float getCurrentFractalEffect() const;
    float getCurrentDepthPerceptionEffect() const;
    float getCurrentDissolveEffect() const;
	float getCurrentRGBDeconvergenceEffect() const;
	float getCurrentRadialBlurEffect() const;
	float getCurrentBloomEffect() const;
	float getCurrentColumnSplitEffect() const;

	// Public getter methods for accessing current tolerance values
    float getCurrentWavyWorldTolerance() const;
    float getCurrentWorldVibrationTolerance() const;
    float getCurrentColorShiftTolerance() const;
    float getCurrentKaleidoscopeTolerance() const;
    float getCurrentNoiseTolerance() const;
    float getCurrentGreyscaleTolerance() const;
    float getCurrentStretchTolerance() const;
    float getCurrentBitcrushTolerance() const;
    float getCurrentWorldScramblerTolerance() const;
    float getCurrentFractalTolerance() const;
    float getCurrentDepthPerceptionTolerance() const;
    float getCurrentDissolveTolerance() const;
	float getCurrentRGBDeconvergenceTolerance() const;
	float getCurrentRadialBlurTolerance() const;
	float getCurrentBloomTolerance() const;
	float getCurrentColumnSplitTolerance() const;

	// 3D Models
    bool carModelLoaded = false;
    Model carModel;

    bool fullbrightEnabled;
    
    // Mouse raycasting for normal vectors
    WallHitInfo currentWallHit;
};

}