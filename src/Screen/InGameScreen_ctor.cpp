#include "Screen/InGameScreen.h"
#include "Game/SelectionPrompt.h"
#include "Game/Sfx.h"
#include "Entity/Catgirl.h"
#include "Entity/Interaction.h"
#include "Entity/DevCar.h"
#include "Inventory/Gun.h"
#include "Inventory/HotPocket.h"
#include "Inventory/GasCan.h"
#include "Inventory/Diaper.h"
#include "Inventory/Shasta12ozCan.h"
#include "Inventory/Drug/Cannabis.h"
#include "Game/Game.h"
#include "Game/HealthBar.h"
#include "Game/MinimapRenderer.h"
#include "Map/New3DRenderer.h"
#include "Map/Map.h"
#include "Map/Building.h"
#include "Mission/Mission.h"
#include "Mission/IdleMission.h"
// Headers for screen types referenced in constructor lambdas
#include "Screen/PortalEditScreen.h"
#include "Screen/InventoryScreen.h"
#include "Screen/MenuScreen.h"
#include "Screen/ExitScreen.h"
#include "Inventory/Inventory.h"
#include "Game/SaveData.h"
#include "Game/Fonts.h"
#include "Game/GameClock.h"
#include "Map/MapFactory.h"
#include "Shader/New3DMainShader.h"
#include "Shader/PostProcessShader.h"
// #include "Shader/TestComputeShader.h"
#include "Shader/OpenGLComputeFunctions.h"
#include "Game/FMRadio.h"

namespace nyaa {

#if DEBUG
InGameScreen::ColoredDebugRect::ColoredDebugRect(float rectLeft, float rectTop, float rectWidth, float rectHeight, unsigned char colR, unsigned char colG, unsigned char colB, float lifeSecs)
	:rectLeft(rectLeft)
	,rectTop(rectTop)
	,rectWidth(rectWidth)
	,rectHeight(rectHeight)
	,colR(colR)
	,colG(colG)
	,colB(colB)
	,colA(255)
	,lifeSecs(lifeSecs)
	,timer(0, 100, true, lifeSecs * 1000, Ease_None)
{

}
#endif

InGameScreen::~InGameScreen()
{
	std::cout << "Destroying IGS" << std::endl;
	delete playerHealthBar;
	delete playerEnergyBar;
	delete playerHungerBar;
	delete playerBladderBar;
	delete playerBasedMeterBar;
#if DEBUG
	delete wavyWorldDrugBar;
	delete worldVibrationDrugBar;
	delete colorShiftDrugBar;
	delete kaleidoscopeDrugBar;
	delete noiseDrugBar;
	delete greyscaleDrugBar;
	delete stretchDrugBar;
	delete bitcrushDrugBar;
	delete worldScramblerDrugBar;
	delete fractalDrugBar;
	delete depthPerceptionDrugBar;
	delete dissolveDrugBar;
	delete rgbDeconvergenceDrugBar;
	delete radialBlurDrugBar;
	delete bloomDrugBar;
#endif
	delete renderer;
	delete setSpawnInter;
	delete newPortalInteraction;
	delete deletePortalInter;
	delete editPortalInter;
    delete spawnCarInter;
    delete spawnBuildingInter;
	delete giveWeaponsInter;
	delete openInvInter;
	delete toggleWallDestructableInter;
	if(curMission)
	{
		delete curMission;
	}
	delete gameClock;
	delete deathPrompt;
	//saveData->save();
	delete mapFactory;
	delete saveData;
	delete sheet;
	delete RainDropsSheet;
	delete carSheet;
	delete postProcessShader;
	delete fmRadio;
	delete invPopup;
#if DEBUG
	// delete testComputeShader;
	delete debugMenuPrompt;
#endif
}

InGameScreen::InGameScreen(int saveDataSlot)
	:saveData(new SaveData(saveDataSlot))
	,mapFactory(new MapFactory())
	,sheet(new Sheet("sheet", 32, 32))
	,player(nullptr)
	,paused(false)
	,initialCam(true)
	,camZoom(3.5f)
	// ,cameraKfAnim(1000)
	,curMission(nullptr)
	,missionFailedTextShown(false)
	,missionPassedTextShown(false)
	,missionFailedTextScaleTimer(0, 100, true, 2000, Ease_OutExpo | Ease_Out | Ease_OutCubic)
	,mcPortalUsed(nullptr)
	,mousePosOnMapX(0)
	,mousePosOnMapY(0)
	,mouseTilePosX(0)
	,mouseTilePosY(0)
	,mouseTilePosOnLeftRelX(0)
	,mouseTilePosOnLeftRelY(0)
	,mouseTilePosInChunkX(0)
	,mouseTilePosInChunkY(0)
	,mouseTilePosMidClosestX(0)
	,mouseTilePosMidClosestY(0)
	,mouseChunkPosX(0)
	,mouseChunkPosY(0)
	,mouseTilePosMiddleX(0)
	,mouseTilePosMiddleY(0)
	,playerHealthBar(new HealthBar("Health", 255, 0, 0))
	,playerEnergyBar(new HealthBar("Energy", 0, 255, 0))
	,playerHungerBar(new HealthBar("Hunger", 255, 170, 20))
	,playerBladderBar(new HealthBar("Bladder", 255, 255, 0))
	,playerBasedMeterBar(new HealthBar("Based", 0, 255, 255))
#if DEBUG
	,wavyWorldDrugBar(new HealthBar("Wavy", 255, 100, 100))
	,worldVibrationDrugBar(new HealthBar("Vibration", 100, 255, 100))
	,colorShiftDrugBar(new HealthBar("Color Shift", 100, 100, 255))
	,kaleidoscopeDrugBar(new HealthBar("Kaleidoscope", 255, 0, 255))
	,noiseDrugBar(new HealthBar("Noise", 200, 200, 200))
	,greyscaleDrugBar(new HealthBar("Greyscale", 128, 128, 128))
	,stretchDrugBar(new HealthBar("Stretch", 255, 165, 0))
	,bitcrushDrugBar(new HealthBar("Bitcrush", 255, 215, 0))
	,worldScramblerDrugBar(new HealthBar("Scrambler", 0, 255, 128))
	,fractalDrugBar(new HealthBar("Fractal", 128, 0, 255))
	,depthPerceptionDrugBar(new HealthBar("Depth", 0, 128, 255))
	,dissolveDrugBar(new HealthBar("Dissolve", 255, 128, 0))
	,rgbDeconvergenceDrugBar(new HealthBar("RGB Decon", 255, 50, 50))
	,radialBlurDrugBar(new HealthBar("Radial Blur", 50, 255, 255))
	,bloomDrugBar(new HealthBar("Bloom", 255, 255, 100))
#endif
	,portalNearPlayer(nullptr)
	,interactableIndex(0)
	,interactableText("", *Fonts::MainFont, 20)
	,raining(false)
	,RainDropsSheet(new Sheet("raindrops", 4, 1))
	,RainVA(sf::PrimitiveType::Quads)
	,carSheet(new Sheet("car_sedan", 8, 1))
#if DEBUG
	,debugText("", *Fonts::OSDFont, 12)
	,tileSelectTileId(0)
	,selectedLayer(1)
	,tileEditOn(false)
	,collisionDisabled(false)
	,collisionDebug(false)
	,velocityDebug(false)
	,grabbedCollAnchor(false)
	,roadEditOn(false)
	,lastEditedRoad(nullptr)
	,grabbedRoadAnchorRoad(nullptr)
	,grabbedRoadAnchorIdx(-1)
	,showHitboxes(true)
	,showDebugMoveCircle(false)
	,placingNewNPC(false)
	,moveEntitiesOn(false)
	,selectedCollisionLineIndex(0)
	,newCollLineState(DrawNewCollLineState_Done)
	,newBuildingWallState(DrawNewBuildingWallState_None)
	,buildingEditOn(false)
	,lastEditedBuilding(nullptr)
	,lastEditedFloor(nullptr)
	,grabbedBuildingWall(nullptr)
	,grabbedBuildingWallAnchorIdx(-1)
	,hoveredBuildingWall(nullptr)
	,hoveredBuildingWallAnchorIdx(-1)
	,grabbedBuildingWallOffset1X(0)
	,grabbedBuildingWallOffset1Y(0)
	,grabbedBuildingWallOffset2X(0)
	,grabbedBuildingWallOffset2Y(0)
	,showEntityOBBDebug(false)
	,debugMenuPrompt(nullptr)
	,debugMenuVisible(false)
	,currentDebugMenuLevel(DebugMenu_Main)
#endif
	,doubleVisionEffect(0.0f)
	// Drug effect value initialization
	,wavyWorldEffect(0.0f)
	,worldVibrationEffect(0.0f)
	,colorShiftEffect(0.0f)
	,kaleidoscopeEffect(0.0f)
	,noiseEffect(0.0f)
	,greyscaleEffect(0.0f)
	,stretchEffect(0.0f)
	,bitcrushEffect(0.0f)
	,worldScramblerEffect(0.0f)
	,fractalEffect(0.0f)
	,depthPerceptionEffect(0.0f)
	,dissolveEffect(0.0f)
	,rgbDeconvergenceEffect(0.0f)
	,radialBlurEffect(0.0f)
	,bloomEffect(0.0f)
#if DEBUG
	,wavyWorldEffectDisplay(0.0f)
	,worldVibrationEffectDisplay(0.0f)
	,colorShiftEffectDisplay(0.0f)
	,kaleidoscopeEffectDisplay(0.0f)
	,noiseEffectDisplay(0.0f)
	,greyscaleEffectDisplay(0.0f)
	,stretchEffectDisplay(0.0f)
	,bitcrushEffectDisplay(0.0f)
	,worldScramblerEffectDisplay(0.0f)
	,fractalEffectDisplay(0.0f)
	,depthPerceptionEffectDisplay(0.0f)
	,dissolveEffectDisplay(0.0f)
	,rgbDeconvergenceEffectDisplay(0.0f)
	,radialBlurEffectDisplay(0.0f)
	,bloomEffectDisplay(0.0f)
#endif
	,deathPrompt(new SelectionPrompt(
		(float) Game::ScreenWidth / 2,
		Game::ScreenHeight - ((float)Game::ScreenHeight / 3),
		150,
		100
	))
	,gameClock(new GameClock())
	,playerAutoWalking(false)
	,playerAutoWalkToPosX(0.0f)
	,playerAutoWalkToPosY(0.0f)
	,cameraFollowingPath(false)
	,cameraTargetPosX(0.0f)
	,cameraTargetPosY(0.0f)
	,cameraVelocityX(0.0f)
	,cameraVelocityY(0.0f)
	,cameraFollowDelay(0.8f)
	,cameraFollowSmoothness(0.05f)
	,currentCameraPathIndex(0)
	,minimap(new MinimapRenderer(120.0f, 120.0f, Game::ScreenWidth - 150.0f, Game::ScreenHeight - 150.0f))
	,renderer(new New3DRenderer())
	,postProcessShader(new PostProcessShader())
	,fmRadio(new FMRadio())
	,plrOnScreen(false)
	,windowWasFocused(true)
	,radioDisplay({ "", 1.0f, 0.0f, false })
// #if DEBUG
// 	,testComputeShader(new TestComputeShader())
// #endif
{
	IGS = this;

	invPopup = new InventoryPopup();

    fmRadio->LoadStation(RadioStation::Station_All, false);
	
	//Util::PrintLnFormat("IGS on slot #%d", G->selectedSaveSlot + 1);
	sf::ContextSettings settings;
	settings.depthBits = 24;
	rendTex.create(Game::ScreenWidth, Game::ScreenHeight, settings);
	rendTex.setSmooth(true);
	rendTexSp.setTexture(rendTex.getTexture(), true);
	
	bloomTex.create(Game::ScreenWidth, Game::ScreenHeight);

	entityGuiTex.create(Game::ScreenWidth, Game::ScreenHeight);
	entityGuiTexSp.setTexture(entityGuiTex.getTexture(), true);

	arrowDownTex = &TextureMan::tman->get("arrowDown");

	setSpawnInter = new Interaction(nullptr, "Set spawn point", [](Interaction* i)
	{

		return true;
	});

	newPortalInteraction = new Interaction(nullptr, "Create new portal", [](Interaction* i) -> bool
	{
		auto& p = IGS->player;
		auto tp = sf::Vector2i{};
		p->getTileInFront(Direction_Default, &tp.x, &tp.y);
		/* auto portal =  */p->hostMap->addPortal(tp.x, tp.y);
		return true;
	});
	newPortalInteraction->requireHoldMs = 500;

	deletePortalInter = new Interaction(nullptr, "Delete portal", [](Interaction* i) -> bool
	{
		IGS->player->hostMap->delPortal(IGS->portalNearPlayer);
		IGS->portalNearPlayer = nullptr;
		return true;
	});
	deletePortalInter->requireHoldMs = 2000;

	editPortalInter = new Interaction(nullptr, "Edit portal", [](Interaction* i) -> bool
	{
		PortalEditScreen::Instance->editingPortal = IGS->portalNearPlayer;
		PortalEditScreen::Instance->switchTo();
		return true;
	});

    spawnCarInter = new Interaction(nullptr, "Spawn car", [](Interaction* i) -> bool
    {
        auto car = new DevCar();
        IGS->player->hostMap->addEnt(car, true);//set as mission spawned even though its not part of the mission
        auto& p = IGS->player;
        auto tp = sf::Vector2i{};
        p->getTileInFront(Direction_Default, &tp.x, &tp.y);
        car->setPosTile(tp.x, tp.y);
        return true;
    });

    spawnBuildingInter = new Interaction(nullptr, "Spawn building", [](Interaction* i) -> bool
    {
        auto& p = IGS->player;
        auto tp = sf::Vector2i{};
        p->getTileInFront(Direction_Default, &tp.x, &tp.y);

        // Create building at tile position
        auto building = new Building();
        building->name = "Spawned Building";
        building->posX = tp.x * Map::TileSizePixels;
        building->posY = tp.y * Map::TileSizePixels;

        // Create 4 connected walls to form a square room
        float wallSize = 128.0f;
        // Wall 1: bottom edge
        building->floors[0]->walls.push_back(BuildingWall(0, 0, wallSize, 0));
        // Wall 2: right edge
        building->floors[0]->walls.push_back(BuildingWall(wallSize, 0, wallSize, wallSize));
        // Wall 3: top edge
        building->floors[0]->walls.push_back(BuildingWall(wallSize, wallSize, 0, wallSize));
        // Wall 4: left edge
        building->floors[0]->walls.push_back(BuildingWall(0, wallSize, 0, 0));

        // Add building to map and spawn physics entities
        p->hostMap->buildings.push_back(building);
        building->spawnWallEntities(p->hostMap);

        return true;
    });

    giveWeaponsInter = new Interaction(nullptr, "Give weapons", [](Interaction* i) -> bool
	{
		auto gun = Gun::Create(GunType::Pistol);
		IGS->player->inv->equippedItem = gun;
		IGS->player->inv->addItem(gun);
		return true;
	});
	giveWeaponsInter->requireHoldMs = 1000;

	openInvInter = new Interaction(nullptr, "Open inventory", [](Interaction* i) -> bool
	{
		InventoryScreen::Instance->switchTo();
		return true;
	});

	toggleWallDestructableInter = new Interaction(nullptr, "Toggle destructable", [](Interaction* i) -> bool
	{
		if (IGS->hoveredBuildingWall)
		{
			IGS->hoveredBuildingWall->destructable = !IGS->hoveredBuildingWall->destructable;
			std::string msg = IGS->hoveredBuildingWall->destructable ? "Wall is now DESTRUCTABLE" : "Wall is now INDESTRUCTABLE";
			sf::Color msgColor = IGS->hoveredBuildingWall->destructable ? sf::Color::Green : sf::Color::Red;
			IGS->showTemporaryMessage(msg, 2.0f, msgColor);
			Sfx::Open->play(false, 0.3f);
		}
		return true;
	});

	auto m = mapFactory->add(new Map("basement", *sheet));
	auto kitchen = mapFactory->add(new Map("kitchen", *sheet));
	auto outdoors = mapFactory->add(new Map("outdoors", *sheet));

	auto hairCol = sf::Color(53, 225, 224);
	player = new Catgirl(
		255, 255, 255, 255, 
		194, 116, 102, 255, 
		hairCol.r, hairCol.g, hairCol.b, hairCol.a,
		156, 88, 213, 255, 
		hairCol.r, hairCol.g, hairCol.b, hairCol.a,
		255, 255, 255, 255
	);
	m->addEnt(player);

	playerHealthBar->realVal = &player->health;
	playerEnergyBar->realVal = &player->energy;
	playerHungerBar->realVal = &player->hunger;
	playerBladderBar->realVal = &player->bladder;
	playerBasedMeterBar->realVal = &player->basedMeter;
	
	// Link drug effect health bars to their display value storage (for softmax scaling)
	wavyWorldDrugBar->realVal = &wavyWorldEffectDisplay;
	worldVibrationDrugBar->realVal = &worldVibrationEffectDisplay;
	colorShiftDrugBar->realVal = &colorShiftEffectDisplay;
	kaleidoscopeDrugBar->realVal = &kaleidoscopeEffectDisplay;
	noiseDrugBar->realVal = &noiseEffectDisplay;
	greyscaleDrugBar->realVal = &greyscaleEffectDisplay;
	stretchDrugBar->realVal = &stretchEffectDisplay;
	bitcrushDrugBar->realVal = &bitcrushEffectDisplay;
	worldScramblerDrugBar->realVal = &worldScramblerEffectDisplay;
	fractalDrugBar->realVal = &fractalEffectDisplay;
	depthPerceptionDrugBar->realVal = &depthPerceptionEffectDisplay;
	dissolveDrugBar->realVal = &dissolveEffectDisplay;
	rgbDeconvergenceDrugBar->realVal = &rgbDeconvergenceEffectDisplay;
	radialBlurDrugBar->realVal = &radialBlurEffectDisplay;
	bloomDrugBar->realVal = &bloomEffectDisplay;

	// auto& kf1 = cameraKfAnim.add();
	// kf1.timeMilliseconds = 0;
	// kf1.easing = Ease_OutCirc;
	// auto& kf2 = cameraKfAnim.add();
	// kf2.timeMilliseconds = cameraKfAnim.lengthMilliseconds;

	interactableText.setFillColor(sf::Color::Green);
	interactableText.setOutlineThickness(1);
	interactableText.setOutlineColor(sf::Color::Black);
	interactableText.setStyle(sf::Text::Style::Underlined);

	deathPrompt->add("main menu", [](SelectionPrompt& prompt, SelectionPrompt::Selection& sel, Direction dir)
	{
		if(dir != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		MenuScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});

	deathPrompt->add("exit game", [](SelectionPrompt& p, SelectionPrompt::Selection& s, Direction d) -> SelPResp
	{
		if(d != Direction_Center)
		{
			return SelPResp_Neutral;
		}
		ExitScreen::Instance->switchTo();
		return SelPResp_DidSomething;
	});

	// Add player-following light
	if (auto* new3dRenderer = dynamic_cast<New3DRenderer*>(renderer)) {
		new3dRenderer->m_new3DMainShader->addPlayerFollowLight();
	}

	//FMRadio::ScanRes();

#if DEBUG
	debugText.setFillColor(sf::Color(0, 255, 0, 100));
	debugText.setOutlineColor(sf::Color(0, 0, 0, 80));
	debugText.setOutlineThickness(3);
	debugText.setPosition(sf::Vector2f(115, 25));
	//IGS->setMission(LuaMission::New("mission1"));
	IGS->setMission(new IdleMission());

	// Create debug menu SelectionPrompt centered on screen like MenuScreen
	// rectLeft is the center X position since text origin is centered
	float centerX = (float)Game::ScreenWidth / 2.0f;
	float topY = (float)Game::ScreenHeight / 2.0f - 170.0f;
	debugMenuPrompt = new SelectionPrompt(centerX, topY, 200, 350, true);
	
	// Initialize main debug menu
	setupDebugMainMenu();
#endif
}

#if DEBUG
void InGameScreen::setupDebugMainMenu()
{
	debugMenuPrompt->clearSelections();
	debugMenuPrompt->setBreadcrumb("Debug Menu");
	currentDebugMenuLevel = DebugMenu_Main;
	
	debugMenuPrompt->add("Close Menu", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->debugMenuVisible = false;
		Sfx::Close->play();
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Visual Debug >", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->setupVisualDebugMenu();
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Map Editing >", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->setupMapEditingMenu();
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Player & World >", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->setupPlayerWorldMenu();
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Give Items >", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->setupItemsMenu();
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
}

void InGameScreen::setupVisualDebugMenu()
{
	debugMenuPrompt->clearSelections();
	debugMenuPrompt->setBreadcrumb("Debug > Visual Debug");
	currentDebugMenuLevel = DebugMenu_VisualDebug;
	
	debugMenuPrompt->add("< Back to Main", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->setupDebugMainMenu();
		Sfx::Close->play();
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Toggle Hitbox Display", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->showHitboxes = !IGS->showHitboxes;
		if (IGS->showHitboxes) Sfx::Open->play(false, 0.3f);
		else Sfx::Close->play();
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Toggle Collision Debug", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->collisionDebug = !IGS->collisionDebug;
		if (IGS->collisionDebug) Sfx::Open->play(false, 0.3f);
		else Sfx::Close->play();
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Toggle Entity OBB Debug", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->showEntityOBBDebug = !IGS->showEntityOBBDebug;
		if (IGS->showEntityOBBDebug) Sfx::Open->play(false, 0.3f);
		else Sfx::Close->play();
		return SelPResp_DidSomething;
	});
}

void InGameScreen::setupMapEditingMenu()
{
	debugMenuPrompt->clearSelections();
	debugMenuPrompt->setBreadcrumb("Debug > Map Editing");
	currentDebugMenuLevel = DebugMenu_MapEditing;
	
	debugMenuPrompt->add("< Back to Main", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->setupDebugMainMenu();
		Sfx::Close->play();
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Toggle Tile Edit", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->tileEditOn = !IGS->tileEditOn;
		if (IGS->tileEditOn) Sfx::Open->play(false, 0.3f);
		else Sfx::Close->play();
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Toggle Walk Through Walls", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->collisionDisabled = !IGS->collisionDisabled;
		if (IGS->collisionDisabled) Sfx::Open->play(false, 0.3f);
		else Sfx::Close->play();
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Toggle NPC Placement Mode", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->placingNewNPC = !IGS->placingNewNPC;
		if (IGS->placingNewNPC) Sfx::Open->play(false, 0.3f);
		else Sfx::Close->play();
		return SelPResp_DidSomething;
	});

	debugMenuPrompt->add("Toggle Building Edit", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->buildingEditOn = !IGS->buildingEditOn;
		if (IGS->buildingEditOn) Sfx::Open->play(false, 0.3f);
		else Sfx::Close->play();
		return SelPResp_DidSomething;
	});

	debugMenuPrompt->add("Add Building Wall", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->buildingEditOn = true;
		IGS->newBuildingWallState = InGameScreen::DrawNewBuildingWallState_FirstPoint;
		IGS->debugMenuVisible = false;
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Add Collision Line", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		auto cm = IGS->mapFactory->getCurrentMap();
		IGS->newCollLineState = DrawNewCollLineState_FirstPoint;
		cm->collLines.push_back(new CollisionLine(IGS->mousePosOnMapX, IGS->mousePosOnMapY, IGS->mousePosOnMapX, IGS->mousePosOnMapY));
		IGS->collisionDebug = true;
		IGS->selectedCollisionLineIndex = cm->collLines.size() - 1;
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
}

void InGameScreen::setupPlayerWorldMenu()
{
	debugMenuPrompt->clearSelections();
	debugMenuPrompt->setBreadcrumb("Debug > Player & World");
	currentDebugMenuLevel = DebugMenu_PlayerWorld;
	
	debugMenuPrompt->add("< Back to Main", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->setupDebugMainMenu();
		Sfx::Close->play();
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Fill Player Bladder", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->player->bladder = 100;
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Toggle Rain", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->raining = !IGS->raining;
		if (IGS->raining) Sfx::Open->play(false, 0.3f);
		else {
			Sfx::Close->play();
			IGS->RainDrops.clear();
		}
		return SelPResp_DidSomething;
	});
}

void InGameScreen::setupItemsMenu()
{
	debugMenuPrompt->clearSelections();
	debugMenuPrompt->setBreadcrumb("Debug > Give Items");
	currentDebugMenuLevel = DebugMenu_Items;
	
	debugMenuPrompt->add("< Back to Main", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		IGS->setupDebugMainMenu();
		Sfx::Close->play();
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Give Pistol", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		auto gun = Gun::Create(GunType::Pistol);
		IGS->player->inv->addItem(gun);
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Give Rifle", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		auto gun = Gun::Create(GunType::Rifle);
		IGS->player->inv->addItem(gun);
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Give Hot Pocket", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		auto item = new HotPocket();
		IGS->player->inv->addItem(item);
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Give Gas Can", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		auto item = new GasCan();
		IGS->player->inv->addItem(item);
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Give Diaper", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		auto item = new Diaper();
		IGS->player->inv->addItem(item);
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Give Shasta Can", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		auto item = new Shasta12ozCan();
		IGS->player->inv->addItem(item);
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
	
	debugMenuPrompt->add("Give Cannabis", [](SelectionPrompt& sp, SelectionPrompt::Selection& sel, Direction dir) -> SelPResp {
		if (dir != Direction_Center) return SelPResp_Neutral;
		auto item = new Cannabis();
		IGS->player->inv->addItem(item);
		Sfx::Open->play(false, 0.3f);
		return SelPResp_DidSomething;
	});
}

void InGameScreen::switchDebugMenuLevel(DebugMenuLevel newLevel)
{
	switch (newLevel) {
	case DebugMenu_Main:
		setupDebugMainMenu();
		break;
	case DebugMenu_VisualDebug:
		setupVisualDebugMenu();
		break;
	case DebugMenu_MapEditing:
		setupMapEditingMenu();
		break;
	case DebugMenu_PlayerWorld:
		setupPlayerWorldMenu();
		break;
	case DebugMenu_Items:
		setupItemsMenu();
		break;
	}
#endif
}

} // namespace nyaa
