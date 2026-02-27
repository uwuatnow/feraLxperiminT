#pragma once
#include "Game/Globals.h"

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include "Screen/Screen.h"
#include "Game/DialogueBox.h"
#include "Game/Animation.h"
#include "Game/Timer.h"
//#include "Entity/Entity.h"
#include "Map/RainDrop.h"
#include "Texture/TextureMan.h"
#include <thread>
#include <atomic>
#include "Game/GuiWidget.h"

namespace nyaa {

class Item;

class InGameScreen : public Screen
{
public:
	class InventoryPopup : public GuiWidget
	{
	public:
		static const int GRID_WIDTH = 5;
		static const int GRID_HEIGHT = 3;
		static const int SLOT_SIZE = 32;
		static const int SLOT_SPACING = 2;
		static const int GRID_START_X = 100;
		static const int GRID_START_Y = 100;

		int m_SelectedSlot = 0;
		int m_ScrollOffset = 0;
		float m_ItemAnimTimer = 0.f;
		Item* m_DraggedItem = nullptr;
		int m_DraggedFromIndex = -1;
		int m_DragOffsetX = 0;
		int m_DragOffsetY = 0;

		void draw(RendTarget* renderTarget, Actor* player);
		void handleInput(Actor* player);
		int getSlotAtPosition(int posX, int posY) const;
		int getSelectedItemIndex() const;
	};

public:
	InGameScreen(int saveDataSlot);
	
	virtual ~InGameScreen();

public:
    void doTick(RendTarget* renderTarget) override;
    
    bool onEnter() override;
    
    bool onLeave() override;
    
    void setMission(class Mission* m);
	
	void switchMaps(std::string toMapName);
	
	void loadChunksNearPlayer();
	
	bool isPointInView(float pointX, float pointY);
	
	bool doesIntersectView(float rectLeft, float rectTop, float rectWidth, float rectHeight);
	
	void getViewRect(float* outLeft, float* outTop, float* outWidth, float* outHeight);
	
	void mapPosToScreenPos(float mapPosX, float mapPosY, int* outX, int* outY);

	void addInteractablePriority(class Interaction* inter);

	//call from other screen tick toupdate in-game logic while another screen is active
	void otherScreenUpdate();

#if DEBUG
	// Debug menu navigation state
	enum DebugMenuLevel {
		DebugMenu_Main,
		DebugMenu_VisualDebug,
		DebugMenu_MapEditing,
		DebugMenu_PlayerWorld,
		DebugMenu_Items
	};
	
	// Debug menu setup functions
	void setupDebugMainMenu();
	void setupVisualDebugMenu();
	void setupMapEditingMenu();
	void setupPlayerWorldMenu();
	void setupItemsMenu();
	void switchDebugMenuLevel(DebugMenuLevel newLevel);
#endif

public:
	class SaveData* saveData;
	class MapFactory* mapFactory;
	class Sheet* sheet;
	class Actor* player;
	TextureMan::Tex* arrowDownTex;
	sf::View worldView;
	bool paused;
	bool initialCam;
	float camZoom;
	Clock camInitialZoomClock;
	// Anim cameraKfAnim;
	class Mission* curMission;
	bool missionFailedTextShown;
	bool missionPassedTextShown;
	Timer missionFailedTextScaleTimer;
	std::string switchToMapName;
	class Portal* mcPortalUsed;
	sf::Sprite tileSheetSp;
	double mousePosOnMapX;
	double mousePosOnMapY;
	int mouseTilePosX;
	int mouseTilePosY;
	int mouseTilePosOnLeftRelX;
	int mouseTilePosOnLeftRelY;
	int mouseTilePosInChunkX;
	int mouseTilePosInChunkY;
	int mouseTilePosMidClosestX;
	int mouseTilePosMidClosestY;
	int mouseChunkPosX;
	int mouseChunkPosY;
	int mouseTilePosMiddleX;
	int mouseTilePosMiddleY;
	DialogueBox dbox;
	class HealthBar* playerHealthBar;
	class HealthBar* playerEnergyBar;
	class HealthBar* playerHungerBar;
	class HealthBar* playerBladderBar;
	class HealthBar* playerBasedMeterBar;
#if DEBUG
	// Drug effect health bars
	class HealthBar* wavyWorldDrugBar;
	class HealthBar* worldVibrationDrugBar;
	class HealthBar* colorShiftDrugBar;
	class HealthBar* kaleidoscopeDrugBar;
	class HealthBar* noiseDrugBar;
	class HealthBar* greyscaleDrugBar;
	class HealthBar* stretchDrugBar;
	class HealthBar* bitcrushDrugBar;
	class HealthBar* worldScramblerDrugBar;
	class HealthBar* fractalDrugBar;
	class HealthBar* depthPerceptionDrugBar;
	class HealthBar* dissolveDrugBar;
	class HealthBar* rgbDeconvergenceDrugBar;
	class HealthBar* radialBlurDrugBar;
	class HealthBar* bloomDrugBar;
#endif
	std::vector<Interaction*> interactables; /* cleared at the end of every frame */
	Interaction* newPortalInteraction;
	Interaction* deletePortalInter;
	Interaction* editPortalInter;
	Interaction* setSpawnInter;
    Interaction* spawnCarInter;
    Interaction* spawnBuildingInter;
	Interaction* giveWeaponsInter;
	Interaction* openInvInter;
	Interaction* toggleWallDestructableInter;
	class Portal* portalNearPlayer;
	size_t interactableIndex;
	sf::Text interactableText;
	std::vector<class Entity*> getAllMissionEnts();
	sf::RenderTexture rendTex;
	sf::Sprite rendTexSp;
	sf::RenderTexture entityGuiTex;
	sf::Sprite entityGuiTexSp;
	unsigned int winSizeFixedX;
	unsigned int winSizeFixedY;
	bool raining;
	class Sheet* RainDropsSheet;
	sf::VertexArray RainVA;
	std::vector<RainDrop> RainDrops;
	Timer RainDropT;
	class Sheet* carSheet;

	std::string stats;
	
#if DEBUG
	class ColoredDebugRect
	{
	public:
		ColoredDebugRect(float rectLeft, float rectTop, float rectWidth, float rectHeight, 
			unsigned char colR = 255, unsigned char colG = 0, unsigned char colB = 0, float lifeSecs = 1.5f);

	public:
		float rectLeft, rectTop, rectWidth, rectHeight;
		unsigned char colR;
		unsigned char colG;
		unsigned char colB;
		unsigned char colA;
		float lifeSecs;
		Timer timer;
	};
	std::vector<ColoredDebugRect> debugRects;
	enum DrawNewCollLineState
	{
		DrawNewCollLineState_Done,
		DrawNewCollLineState_Deleting,
		DrawNewCollLineState_FirstPoint,
		DrawNewCollLineState_SecondPoint
	};
	sf::Text debugText;
	unsigned int tileSelectTileId;
	int selectedLayer;
	bool tileEditOn;
	bool collisionDisabled;
	bool collisionDebug;
	bool velocityDebug;
	bool grabbedCollAnchor;
	bool roadEditOn;
	class Road* lastEditedRoad;
	class Road* grabbedRoadAnchorRoad;
	int grabbedRoadAnchorIdx;
	bool showHitboxes;
	bool showDebugMoveCircle;
	bool placingNewNPC;
	bool moveEntitiesOn; //igs->player->isControllable must be false
	std::vector<struct CollisionLinePoint*> movingV2fps;
	unsigned int selectedCollisionLineIndex;
	double newPointPosX, newPointPosY;
	DrawNewCollLineState newCollLineState;
	enum DrawNewBuildingWallState
	{
		DrawNewBuildingWallState_None,
		DrawNewBuildingWallState_FirstPoint,
		DrawNewBuildingWallState_SecondPoint
	};
	DrawNewBuildingWallState newBuildingWallState;
	bool buildingEditOn;
	class Building* lastEditedBuilding;
	class Floor* lastEditedFloor;
	class BuildingWall* grabbedBuildingWall;
	int grabbedBuildingWallAnchorIdx;
	class BuildingWall* hoveredBuildingWall;
	int hoveredBuildingWallAnchorIdx;
	double grabbedBuildingWallOffset1X, grabbedBuildingWallOffset1Y;
	double grabbedBuildingWallOffset2X, grabbedBuildingWallOffset2Y;
	bool showEntityOBBDebug;
	class SelectionPrompt* debugMenuPrompt;
	bool debugMenuVisible;
	DebugMenuLevel currentDebugMenuLevel;
	bool globalEditON; // This now only works with walls 2026-02-06
#endif
    float doubleVisionEffect;
	
#if DEBUG
	// Drug effect value storage for health bars
	float wavyWorldEffect;
	float worldVibrationEffect;
	float colorShiftEffect;
	float kaleidoscopeEffect;
	float noiseEffect;
	float greyscaleEffect;
	float stretchEffect;
	float bitcrushEffect;
	float worldScramblerEffect;
	float fractalEffect;
	float depthPerceptionEffect;
	float dissolveEffect;
	float rgbDeconvergenceEffect;
	float radialBlurEffect;
	float bloomEffect;
	
	// Scaled display values for softmax-style rendering
	float wavyWorldEffectDisplay;
	float worldVibrationEffectDisplay;
	float colorShiftEffectDisplay;
	float kaleidoscopeEffectDisplay;
	float noiseEffectDisplay;
	float greyscaleEffectDisplay;
	float stretchEffectDisplay;
	float bitcrushEffectDisplay;
	float worldScramblerEffectDisplay;
	float fractalEffectDisplay;
	float depthPerceptionEffectDisplay;
	float dissolveEffectDisplay;
	float rgbDeconvergenceEffectDisplay;
	float radialBlurEffectDisplay;
	float bloomEffectDisplay;
#endif	
	class SelectionPrompt* deathPrompt;
	class GameClock* gameClock;
	
	bool playerAutoWalking;
	double playerAutoWalkToPosX, playerAutoWalkToPosY;
	
	// Smooth delayed camera following for auto walk
	bool cameraFollowingPath;
	double cameraTargetPosX, cameraTargetPosY;
	double cameraVelocityX, cameraVelocityY;
	float cameraFollowDelay;
	float cameraFollowSmoothness;
	struct CamPathPoint { double x, y; };
	std::vector<CamPathPoint> cameraPathPoints;
	size_t currentCameraPathIndex;
	
	class MinimapRenderer* minimap;

	class MapRenderer* renderer;
	bool showInventoryPopup = false;
	InventoryPopup* invPopup;
	class PostProcessShader* postProcessShader;
	sf::RenderTexture bloomTex;
	class FMRadio* fmRadio;
	bool plrOnScreen;
	bool windowWasFocused;

	// Radio station display
	struct RadioStationDisplay {
		std::string stationName;
		float alpha;
		float displayTime;
		bool active;
	};
	RadioStationDisplay radioDisplay;
	void showRadioStation(const std::string& stationName);

	// Temporary message display system
	struct TemporaryMessage {
		std::string message;
		float displayTime;
		float duration;
		bool active;
		sf::Color color;
		
		TemporaryMessage() : displayTime(0.0f), duration(2.0f), active(false), color(sf::Color::White) {}
	};
	TemporaryMessage tempMessage;
	void showTemporaryMessage(const std::string& message, float duration = 2.0f, sf::Color color = sf::Color::White);

// #if DEBUG
// 	class TestComputeShader* testComputeShader;
// #endif

};

extern InGameScreen* IGS;

}
