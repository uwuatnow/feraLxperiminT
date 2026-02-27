#include "Screen/InGameScreen.h"
#include "Entity/Catgirl.h"
#include "Game/HealthBar.h"
#include "Entity/Interaction.h"
#include "Entity/DevCar.h"
#include "Inventory/Gun.h"
#include <SFML/System/Vector2.hpp>
#include "Screen/InitMissionScreen.h"
#include "Game/Util.h"
#include <SFML/Window/Mouse.hpp>
#include "Screen/PauseScreen.h"
#include "Screen/MapSwitchScreen.h"
#include "Screen/MapScreen.h"
#include "Entity/Road.h"
#include "Map/Map.h"
#include "Map/MapFactory.h"
#include "Map/Building.h"
#include "Screen/MenuScreen.h"
#include "Screen/ExitScreen.h"
#include "Map/Pathfinding.h"
#include "Screen/PortalEditScreen.h"
#include "Inventory/Inventory.h"
#include "Screen/InventoryScreen.h"
#include "Mission/IdleMission.h"
#include "Game/Game.h"
#include "Inventory/HotPocket.h"
#include "Game/Controller.h"
#include "Game/Sfx.h"
#include "Game/Mouse.h"
#include "Game/MinimapRenderer.h"
#include "Map/New3DRenderer.h"
#include "Shader/PostProcessShader.h"
#include "Game/RectangleButton.h"
#include "Game/Fonts.h"
#include "Game/GameClock.h"
#include "Game/SelectionPrompt.h"
// #include "Shader/TestComputeShader.h"

namespace nyaa {

InGameScreen* IGS = nullptr;

void InGameScreen::doTick(RendTarget* renderTarget)
{
	stats.clear();

	// Check for FLC bypass
	Sfx::Engine.bypassFLC = Kb::IsKeyDown(KB::RShift);

	auto cm = mapFactory->getCurrentMap();

	if (G->curScreenFramesPassed == 0)
	{
		missionFailedTextShown = false;
	}

	if (!curMission->wasInited) {
		if (G->curScreenFramesPassed == 0) {
			InitMissionScreen::DoInit();
		}
		else InitMissionScreen::Instance->switchTo();
	}

	plrOnScreen = false;
	if (New3DRenderer* renderer3D = dynamic_cast<New3DRenderer*>(renderer))
	{
		int scrX = 0, scrY = 0;
		renderer3D->worldToScreen(player->posX, player->posY, rendTex, &scrX, &scrY);
		plrOnScreen = scrX >= 0 && scrY >= 0 
			&& scrX < Game::ScreenWidth && scrY < Game::ScreenHeight;
	}
	else
	{
		plrOnScreen = isPointInView(player->posX, player->posY);
	}
	
	float moveDist = Util::Lowest(worldView.getSize().x, worldView.getSize().y) * 0.3f;

	// auto& kf1 = cameraKfAnim[0];

	//some of this camera code is the old 2d cam which the new cam may or may not need

	if (G->winResized || initialCam)
	{
		//auto playerChunkPos = sf::Vector2f{};
		//MapChunk::GetPosInWorld(player->chunkPosX, player->chunkPosY, &playerChunkPos.x, &playerChunkPos.y);
		//kf1.rectLeft = playerChunkPos.x;
		//kf1.rectTop = playerChunkPos.y;
		//kf1.rectWidth = (float)Game::ScreenWidth;
		//kf1.rectHeight = (float)Game::ScreenHeight;
		//cameraKfAnim.restart();
		camInitialZoomClock.restart();
	}

	//auto& kf2 = cameraKfAnim[1];

	sf::Vector2f camspc(Game::ScreenWidth / camZoom, Game::ScreenHeight / camZoom);
	
	// Use car position if player is in a car
	double effectivePlayerX = player->posX;
	double effectivePlayerY = player->posY;
	
	if (player->carImInsideOf != nullptr) {
		effectivePlayerX = player->carImInsideOf->posX;
		effectivePlayerY = player->carImInsideOf->posY;
	}
	
	//kf2.rectLeft = effectivePlayerX - (camspc.x / 2);
	//kf2.rectTop = effectivePlayerY - (camspc.y / 2);
	//kf2.rectWidth = camspc.x;
	//kf2.rectHeight = camspc.y;

	//Anim::Kf betweenKF = cameraKfAnim.between();

	//cameraKfAnim.update();
	float cizc = camInitialZoomClock.getElapsedMilliseconds();
	float zoomTime = /* cameraKfAnim.lengthMilliseconds */1000;
	//if (cameraKfAnim.state == Anim::State_Playing) {
	//	worldView.reset(sf::FloatRect{ betweenKF.rectLeft, betweenKF.rectTop, betweenKF.rectWidth, betweenKF.rectHeight });
	//}
	sf::Vector2f camCenterPos = worldView.getCenter();
	
	sf::Vector2f cap = { (float)effectivePlayerX, (float)effectivePlayerY };
	cap.y -= (player->sizeY);
	float distFromCam = Util::Dist(camCenterPos.x, camCenterPos.y, cap.x, cap.y);

	// Handle smooth delayed camera following for auto walk path
	if (cameraFollowingPath && !cameraPathPoints.empty())
	{
		// Use the player's next node for camera positioning
		if (player->astn && player->ast && player->ast->valid())
		{
			// Get the next node the player is walking towards
			PathfindNode* nextNode = player->astn;
			if (nextNode && nextNode->child)
			{
				// Look ahead to the node after next for smoother camera movement
				cameraTargetPosX = nextNode->child->posX;
				cameraTargetPosY = nextNode->child->posY;
			}
			else if (nextNode)
			{
				// Use current target node if no next node available
				cameraTargetPosX = nextNode->posX;
				cameraTargetPosY = nextNode->posY;
			}
		}
		
		// Smooth interpolation towards target - use double precision for stability at large distances
		sf::Vector2f targetDiff = sf::Vector2f{ (float)cameraTargetPosX, (float)cameraTargetPosY } - camCenterPos;
		cameraVelocityX += targetDiff.x * (float)cameraFollowSmoothness;
		cameraVelocityY += targetDiff.y * (float)cameraFollowSmoothness;
		cameraVelocityX *= 0.85f; // Damping
		cameraVelocityY *= 0.85f;
		
		// Apply camera movement with double precision calculations
		double moveX = (double)cameraVelocityX * (double)G->frameDeltaMillis * 0.01;
		double moveY = (double)cameraVelocityY * (double)G->frameDeltaMillis * 0.01;
		worldView.move((float)moveX, (float)moveY);
	}
	else
	{
		// Normal camera following (original logic) - use double precision for stability at large distances
		if (distFromCam > moveDist)
		{
			double xdist = effectivePlayerX - camCenterPos.x;
			double ydist = effectivePlayerY - camCenterPos.y;
			double spd = (double)(player->moveAmountMax * G->frameDeltaMillis / 16);
			//if (!plrOnScreen) spd = 30.0f;
			double moveX = Util::Clamp(xdist, -spd, spd);
			double moveY = Util::Clamp(ydist, -spd, spd);
			worldView.move((float)moveX, (float)moveY);
		}
	}

	if (!curMission->camSettled && cizc >= zoomTime)
	{
		curMission->onCameraSettle();
		curMission->camSettled = true;
		player->isControllable = true;
	}

	if(curMission)
	{
		curMission->doScreenStatsText(stats);
	}

#if DEBUG
	// grabbedCollAnchor reset moved to end of doTick to allow its use in Mouse::LeftRel checks
#endif
	guip = this;

	if ((Kb::IsKeyFirstFrame(KB::I) || (Controller::dpy == 100 && Controller::dpyFrames == 1)) && (guip_eof == this || guip_eof == invPopup))
	{
		showInventoryPopup = !showInventoryPopup;
		if (showInventoryPopup) {
			invPopup->m_SelectedSlot = 0;
			invPopup->m_ScrollOffset = 0;
			invPopup->m_DraggedItem = nullptr;
		}
	}

	if (showInventoryPopup) {
		guip = invPopup;
	}

	auto& vpp = worldView.getSize();
	float zoom = vpp.x / Game::ScreenWidth;
	auto vp = sf::Vector2f(worldView.getCenter().x - (worldView.getSize().x / 2),
		worldView.getCenter().y - (worldView.getSize().y / 2));
	/* auto wvs = worldView.getSize(); */
	sf::Vector2i mp = sf::Vector2i{ Mouse::Pos_X, Mouse::Pos_Y };
	mp = sf::Vector2i(mp.x * zoom, mp.y * zoom);
	
	// Use 3D coordinate conversion if New3DRenderer is active
	if (New3DRenderer* renderer3D = dynamic_cast<New3DRenderer*>(renderer))
	{
		double s2wX, s2wY;
		renderer3D->screenToWorld(Mouse::Pos_X, Mouse::Pos_Y, rendTex, &s2wX, &s2wY);
		mousePosOnMapX = s2wX;
		mousePosOnMapY = s2wY;
	}
	// else
	// {
	// 	// When player is in a car, use car's position for mouse calculations
	// 	double effectivePlayerX = player->posX;
	// 	double effectivePlayerY = player->posY;
		
	// 	if (player->carImInsideOf != nullptr) {
	// 		effectivePlayerX = player->carImInsideOf->posX;
	// 		effectivePlayerY = player->carImInsideOf->posY;
	// 	}
		
	// 	// Calculate viewport based on effective player position
	// 	auto effectiveVp = sf::Vector2f(worldView.getCenter().x - (worldView.getSize().x / 2),
	// 		worldView.getCenter().y - (worldView.getSize().y / 2));
		
	// 	mousePosOnMapX = mp.x + effectiveVp.x;
	// 	mousePosOnMapY = mp.y + effectiveVp.y;
	// }

	mouseTilePosX = mousePosOnMapX / (float)Map::TileSizePixels;
	mouseTilePosY = mousePosOnMapY / (float)Map::TileSizePixels;

	if (mousePosOnMapX < 0) mouseTilePosX -= 1;
	if (mousePosOnMapY < 0) mouseTilePosY -= 1;
	if (Mouse::LeftRel)
	{
		mouseTilePosOnLeftRelX = mouseTilePosX; 
		mouseTilePosOnLeftRelY = mouseTilePosY;
	}

	mouseTilePosMiddleX = (mousePosOnMapX + (Map::TileSizePixels / 2)) / (float)Map::TileSizePixels;
	mouseTilePosMiddleY = (mousePosOnMapY + (Map::TileSizePixels / 2)) / (float)Map::TileSizePixels;

	if (mousePosOnMapX < 0) mouseTilePosMiddleX -= 1;
	if (mousePosOnMapY < 0) mouseTilePosMiddleY -= 1;

	sf::Vector2f snappedMousePosOnMap
	(
		(float)(mouseTilePosMiddleX * Map::TileSizePixels),
		(float)(mouseTilePosMiddleY * Map::TileSizePixels)
	);
#if DEBUG
	if (Mouse::LeftFrames == 0) movingV2fps.clear();
	if (guip_eof == this && !movingV2fps.empty())
	{
		bool noSnap = Kb::IsKeyDown(KB::LAlt);
		for (auto it = movingV2fps.begin(); it != movingV2fps.end(); ++it) {
			auto v = *it;
			sf::Vector2f nsos = noSnap ? sf::Vector2f{ (float)mousePosOnMapX, (float)mousePosOnMapY } : snappedMousePosOnMap;
			v->x = nsos.x;
			v->y = nsos.y;
		}
		if (Mouse::LeftRel) movingV2fps.clear();
	}
#endif
	mouseTilePosInChunkX = abs(mouseTilePosX) % MapChunk::SizeTiles;
	mouseTilePosInChunkY = abs(mouseTilePosY) % MapChunk::SizeTiles;

	if (mousePosOnMapX < 0) mouseTilePosInChunkX = MapChunk::SizeTiles - mouseTilePosInChunkX;
	if (mousePosOnMapY < 0) mouseTilePosInChunkY = MapChunk::SizeTiles - mouseTilePosInChunkY;

	mouseChunkPosX = mousePosOnMapX / (float)MapChunk::SizePixels;
	mouseChunkPosY = mousePosOnMapY / (float)MapChunk::SizePixels;

	if (mousePosOnMapX < 0) mouseChunkPosX -= 1;
	if (mousePosOnMapY < 0) mouseChunkPosY -= 1;

	mouseTilePosMidClosestX = (mousePosOnMapX + (Map::TileSizePixels / 2)) / (float)Map::TileSizePixels;
	mouseTilePosMidClosestY = (mousePosOnMapY + (Map::TileSizePixels / 2)) / (float)Map::TileSizePixels;
	
	if (mousePosOnMapX < -(Map::TileSizePixels / 2)) mouseTilePosMidClosestX -= 1;
	if (mousePosOnMapY < -(Map::TileSizePixels / 2)) mouseTilePosMidClosestY -= 1;

	// Handle right-click to set auto-walk target
	if(
#if DEBUG
		!tileEditOn && !roadEditOn &&
#endif
		Mouse::RightFrames == 1 
		&& IGS->player->isControllable 
		&& !IGS->dbox.showing
		&& guip_eof == this
	)
	{
		playerAutoWalking = true;
		playerAutoWalkToPosX = mousePosOnMapX;
		playerAutoWalkToPosY = mousePosOnMapY;
		const auto & ca = IGS->player;

		if (ca->carImInsideOf)
		{
			// Car auto-drive
			ca->carImInsideOf->autoDriveTo(playerAutoWalkToPosX, playerAutoWalkToPosY);
			Sfx::Open->play(false, 0.3f);
		}
		else if(playerAutoWalkToPosX != ca->posX || playerAutoWalkToPosY != ca->posY)
		{
			// Initialize new pathfinding (Player)
			if (ca->ast)
			{
				delete ca->ast;
			}

			ca->ast = new Pathfinding();
			ca->ast->loadNodes(ca->hostMap, ca->posX, ca->posY, playerAutoWalkToPosX, playerAutoWalkToPosY);
			ca->ast->solve();
			ca->astn = ca->ast->startNode;
			ca->reSolveAST = false;
			ca->directPath = false;
			
			// Play autowalk start sound
			Sfx::Open->play(false, 0.3f);
			
			// Build camera path for smooth following
			cameraPathPoints.clear();
			if (ca->ast->valid()) {
				PathfindNode* node = ca->ast->startNode;
				while (node) {
					cameraPathPoints.push_back({ node->posX, node->posY });
					node = node->child;
				}
				cameraFollowingPath = true;
				currentCameraPathIndex = 0;
				auto wvc = worldView.getCenter();
				cameraTargetPosX = wvc.x;
				cameraTargetPosY = wvc.y;
			}
		}
	}

	// Handle auto-walking
	if (playerAutoWalking && player && player->isControllable)
	{
		// If player reaches the target or auto-walking is cancelled by movement keys
		bool shouldCancelAutoWalk = 
			(G->inMethod == InputMethod_Keyboard && (Kb::IsKeyDown(KB::W) || Kb::IsKeyDown(KB::A) || 
			 Kb::IsKeyDown(KB::S) || Kb::IsKeyDown(KB::D) || Kb::IsKeyDown(KB::Up) || 
			 Kb::IsKeyDown(KB::Down) || Kb::IsKeyDown(KB::Left) || Kb::IsKeyDown(KB::Right))) /*||
			(G->inMethod == InputMethod_Controller && Controller::GetLeftStick().x != 0 && Controller::GetLeftStick().y != 0)*/;
    
		if (shouldCancelAutoWalk)
		{
			playerAutoWalking = false;
			cameraFollowingPath = false;
			if (player->carImInsideOf) {
				player->carImInsideOf->stopAutoDrive();
			}
		}
		else
		{
			// Perform auto-walking
			bool stillWalking;
			if (player->carImInsideOf) {
				stillWalking = player->carImInsideOf->autoDriveTo(playerAutoWalkToPosX, playerAutoWalkToPosY);
			} else {
				stillWalking = player->autoWalkTo(playerAutoWalkToPosX, playerAutoWalkToPosY);
			}
        
			// If we've reached the destination or auto-walking failed
			if (!stillWalking)
			{
				playerAutoWalking = false;
				cameraFollowingPath = false;
			}
		}
	}

	//rendTex.clear(sf::Color(0, 0, 0, 150));
	rendTex.setView(rendTex.getDefaultView());
	sf::RectangleShape bg(sf::Vector2f(Game::ScreenWidth,Game::ScreenHeight));
	bg.setFillColor(sf::Color::White);
	rendTex.draw(bg);
	rendTex.setView(worldView);

	////////
	/// IN GAME CLOCK (in the top right)
	///////
	gameClock->tick();
	sf::Text clockStrTxt(gameClock->timeStr + Util::Format("\n$%d\n", player->inv->money) + stats, *Fonts::OSDFont);
	clockStrTxt.setFillColor(sf::Color(0, 255, 0));
	clockStrTxt.setOutlineColor(sf::Color::Black);
	clockStrTxt.setOutlineThickness(1.0f);
	//std::cout << gameClock.timeStr << std::endl;

	//update teh current map
 	cm->update();

	loadChunksNearPlayer();

	renderer->render(cm, rendTex);
	initialCam = false;

	curMission->update();
#if DEBUG
	//debug rects cleanup
	debugRects.erase(std::remove_if(debugRects.begin(), debugRects.end(),
		[](ColoredDebugRect& dr) {
			return dr.timer.once(dr.timer.lengthMs);
		}), debugRects.end());
#endif
	if (raining)
	{
		RainDropT.update();
		if (RainDropT.once(16))
		{
			RainDrops.erase(std::remove_if(RainDrops.begin(), RainDrops.end(), [](RainDrop& d)
			{
				return d.y > Game::ScreenHeight || d.life.getElapsedSeconds() > 5;
			}), RainDrops.end());
			RainDrops.emplace_back();
			RainDrops.emplace_back();
			RainDrops.emplace_back();
			RainDrops.emplace_back();
		}
		auto spdm = (G->frameDeltaMillis / (1000 / Game::FPSConstant));
		for (auto& d : RainDrops)
		{
			d.timer.update();
			if (d.frame != 0)
			{
				if (d.frame < 3 && d.timer.once(50))
				{
					d.frame++;
				}
				continue;
			}
			d.y += (8 + (1.0 * (d.dirHoriz / 4))) * spdm;
			d.x += (d.dirHoriz * 2) * spdm;
		}
		RainVA.clear();
		for (auto& d : RainDrops)
		{
			auto tc = sf::Vector2f{};
			RainDropsSheet->getTexCoordsFromId((int)Util::Clamp(d.frame, 0, 3), &tc.x, &tc.y);
			auto sz = 5;
			auto tsX = RainDropsSheet->tileSizePixelsX;
			auto tsY = RainDropsSheet->tileSizePixelsY;
			auto col = sf::Color::Blue;
			col.b = col.a = 255 - Util::ScaleClamped(std::fabs(d.dirHoriz), 0.0, 1.0, 0, 150);
			sf::Vertex v1, v2, v3, v4;
			if (d.dirHoriz < 0)
			{
				v1 = sf::Vertex(sf::Vector2f(d.x - sz,d.y - sz), col, tc);
				v2 = sf::Vertex(sf::Vector2f(d.x + sz,d.y - sz), col, sf::Vector2f(tc.x + tsX,tc.y));
				v3 = sf::Vertex(sf::Vector2f(d.x + sz,d.y + sz), col, sf::Vector2f(tc.x + tsX,tc.y + tsY));
				v4 = sf::Vertex(sf::Vector2f(d.x - sz,d.y + sz), col, sf::Vector2f(tc.x,tc.y + tsY));
			}
			else
			{
				v1 = sf::Vertex(sf::Vector2f(d.x - sz,d.y - sz), col, sf::Vector2f(tc.x + tsX,tc.y));
				v2 = sf::Vertex(sf::Vector2f(d.x + sz,d.y - sz), col, tc);
				v3 = sf::Vertex(sf::Vector2f(d.x + sz,d.y + sz), col, sf::Vector2f(tc.x,tc.y + tsY));
				v4 = sf::Vertex(sf::Vector2f(d.x - sz,d.y + sz), col, sf::Vector2f(tc.x + tsX,tc.y + tsY));
			}
			RainVA.append(v1);
			RainVA.append(v2);
			RainVA.append(v3);
			RainVA.append(v4);
		}
		// Rain collision detection works for both 2D and 3D
		if (dynamic_cast<New3DRenderer*>(renderer) == nullptr) {
			// 2D collision detection
			for (auto& r : cm->RectOfTilesRendered)
			{
				for (auto& d : RainDrops)
				{
					if (d.frame != 0)
					{
						continue;
					}
					if (sf::FloatRect{ r.left, r.top, r.width, r.height }.contains(sf::Vector2f(d.x, d.y)))
					{
						d.frame++;
					}
				}
			}
		} else {
			
		}
		rendTex.setView(rendTex.getDefaultView());
		rendTex.draw(RainVA, &RainDropsSheet->tex);
		rendTex.setView(worldView);
	}

#if DEBUG
	auto front = sf::Vector2i{};
	player->getTileInFront(Direction_Default, &front.x, &front.y);

	if (tileEditOn && guip_eof == this)
	{
		// Skip 2D debug rendering when 3D renderer is active
		if (dynamic_cast<New3DRenderer*>(renderer) == nullptr)
		{
			sf::VertexArray debugHighlightsVA(sf::PrimitiveType::Quads);

			auto mouseTileHighlightPosition
				= sf::Vector2f(mouseTilePosX * Map::TileSizePixels, mouseTilePosY * Map::TileSizePixels);
			auto mouseTileHighlightSize(sf::Vector2f(Map::TileSizePixels, Map::TileSizePixels));
			auto mouseTileHighlightFillColor(sf::Color(255, 255, 50, 150));

			auto mouseChunkHighlightPosition
				= sf::Vector2f(mouseChunkPosX * MapChunk::SizePixels, mouseChunkPosY * MapChunk::SizePixels);
			auto mouseChunkHighlightSize(sf::Vector2f(MapChunk::SizePixels, MapChunk::SizePixels));
			auto mouseChunkHighlightFillColor(sf::Color(255, 255, 255, 40));

			auto tileSheetTileHighlightPosition
				= sf::Vector2f(front.x * Map::TileSizePixels, front.y * Map::TileSizePixels);
			auto tileSheetTileHighlightSize(sf::Vector2f(Map::TileSizePixels, Map::TileSizePixels));
			auto tileSheetTileHighlightFillColor(sf::Color(255, 0, 0, 150));

			debugHighlightsVA.append(sf::Vertex(mouseTileHighlightPosition, mouseTileHighlightFillColor));
			debugHighlightsVA.append(sf::Vertex(sf::Vector2f(mouseTileHighlightPosition.x + mouseTileHighlightSize.x,
				mouseTileHighlightPosition.y), mouseTileHighlightFillColor));
			debugHighlightsVA.append(sf::Vertex(sf::Vector2f(mouseTileHighlightPosition.x + mouseTileHighlightSize.x,
				mouseTileHighlightPosition.y + mouseTileHighlightSize.y), mouseTileHighlightFillColor));
			debugHighlightsVA.append(sf::Vertex(sf::Vector2f(mouseTileHighlightPosition.x,
				mouseTileHighlightPosition.y + mouseTileHighlightSize.y), mouseTileHighlightFillColor));

			debugHighlightsVA.append(sf::Vertex(mouseChunkHighlightPosition, mouseChunkHighlightFillColor));
			debugHighlightsVA.append(sf::Vertex(sf::Vector2f(mouseChunkHighlightPosition.x + mouseChunkHighlightSize.x,
				mouseChunkHighlightPosition.y), mouseChunkHighlightFillColor));
			debugHighlightsVA.append(sf::Vertex(sf::Vector2f(mouseChunkHighlightPosition.x + mouseChunkHighlightSize.x,
				mouseChunkHighlightPosition.y + mouseChunkHighlightSize.y), mouseChunkHighlightFillColor));
			debugHighlightsVA.append(sf::Vertex(sf::Vector2f(mouseChunkHighlightPosition.x,
				mouseChunkHighlightPosition.y + mouseChunkHighlightSize.y), mouseChunkHighlightFillColor));

			debugHighlightsVA.append(sf::Vertex(tileSheetTileHighlightPosition, tileSheetTileHighlightFillColor));
			debugHighlightsVA.append(sf::Vertex(sf::Vector2f(tileSheetTileHighlightPosition.x + tileSheetTileHighlightSize.x,
				tileSheetTileHighlightPosition.y), tileSheetTileHighlightFillColor));
			debugHighlightsVA.append(sf::Vertex(sf::Vector2f(tileSheetTileHighlightPosition.x + tileSheetTileHighlightSize.x,
				tileSheetTileHighlightPosition.y + tileSheetTileHighlightSize.y), tileSheetTileHighlightFillColor));
			debugHighlightsVA.append(sf::Vertex(sf::Vector2f(tileSheetTileHighlightPosition.x,
				tileSheetTileHighlightPosition.y + tileSheetTileHighlightSize.y), tileSheetTileHighlightFillColor));

			rendTex.draw(debugHighlightsVA);
		}

		if (Mouse::Moved && movingV2fps.empty())
		{
			auto c = cm->getChunk(mouseChunkPosX, mouseChunkPosY);
			if (c)
			{
				if (Mouse::LeftFrames >= 1)
				{
					c->set(mouseTilePosInChunkX, mouseTilePosInChunkY,
						selectedLayer, tileSelectTileId);
				}
				if (Mouse::RightFrames)
				{
					c->set(mouseTilePosInChunkX,
						mouseTilePosInChunkY, selectedLayer, 0);
				}
			}
		}
	}
#endif
	rendTex.display();

	// Apply post-processing effects if enabled
	bool anyPostProcess = false;
	if (New3DRenderer* renderer3D = dynamic_cast<New3DRenderer*>(renderer)) {
		if (renderer3D->getCurrentBloomEffect() > 0.0f) {
			postProcessShader->setBloomIntensity(renderer3D->getCurrentBloomEffect());
			anyPostProcess = true;
		} else {
			postProcessShader->setBloomIntensity(0.0f);
		}
		
		if (renderer3D->getCurrentRGBDeconvergenceEffect() > 0.0f) {
			postProcessShader->setRgbDeconvergenceEffect(renderer3D->getCurrentRGBDeconvergenceEffect());
			anyPostProcess = true;
		} else {
			postProcessShader->setRgbDeconvergenceEffect(0.0f);
		}

	if (renderer3D->getCurrentRadialBlurEffect() > 0.0f) {
			postProcessShader->setRadialBlurEffect(renderer3D->getCurrentRadialBlurEffect());
			anyPostProcess = true;
		} else {
			postProcessShader->setRadialBlurEffect(0.0f);
		}

		if (anyPostProcess) {
			postProcessShader->setTime(G->screenSwitchClock.getElapsedSeconds());
			postProcessShader->setResolution((float)Game::ScreenWidth, (float)Game::ScreenHeight);
			postProcessShader->update();
		}

	}
	rendTexSp.setTexture(rendTex.getTexture(), true);

	////////
	/// HUD
	////////
	auto rtR = sf::IntRect(0, 0, Game::ScreenWidth, Game::ScreenHeight);
	rendTexSp.setTextureRect(rtR);
	rendTexSp.setColor(sf::Color(255, 255, 255, 200));
	winSizeFixedX = (unsigned)Game::ScreenWidth;
	winSizeFixedY = (unsigned)Game::ScreenHeight;
	if (winSizeFixedX * 3 > winSizeFixedY * 4)
		winSizeFixedX = winSizeFixedY * 4 / 3;
	else
		winSizeFixedY = winSizeFixedX * 3 / 4;

	sf::Shader* activeShader = anyPostProcess ? &postProcessShader->shader : nullptr;
	
	// Check for column split effect
	float columnSplitEffect = 0.0f;
	if (New3DRenderer* renderer3D = dynamic_cast<New3DRenderer*>(renderer)) {
		columnSplitEffect = renderer3D->getCurrentColumnSplitEffect();
	}
	
	if (columnSplitEffect > 0.0f) {
		// Draw 6 separate columns with sine wave offsets
		const int numColumns = 6;
		int colWidth = Game::ScreenWidth / numColumns;
		float time = G->screenSwitchClock.getElapsedSeconds();
		
		// Chaos factor increases with intensity
		float chaosFactor = columnSplitEffect;
		
		for (int i = 0; i < numColumns; i++) {
			// Calculate column position and texture rect
			int colX = i * colWidth;
			sf::IntRect colRect(colX, 0, colWidth, Game::ScreenHeight);
			
			// Different phase for each column
			float colPhase = i * 0.8f;
			
		// Chaotic speed increases with intensity
			float baseSpeed = 2.0f + chaosFactor * 8.0f;
			float chaosSpeed = sin(time * 0.5f + colPhase) * 2.0f * chaosFactor;

			// Chaotic amplitude increases with intensity - MUCH higher now
			float baseAmplitude = 30.0f + chaosFactor * 120.0f; // pixels: 30-150 range
			float chaosAmplitude = sin(time * 1.3f + colPhase * 1.5f) * 40.0f * chaosFactor; // up to 40 additional pixels

			// Calculate Y offset using sine wave with chaos
			float yOffset = sin(time * baseSpeed + colPhase + chaosSpeed) * (baseAmplitude + chaosAmplitude);

			// Add secondary wave for more chaos at high intensity
			if (chaosFactor > 0.5f) {
				yOffset += sin(time * baseSpeed * 1.5f + colPhase * 2.0f) * baseAmplitude * 0.7f * chaosFactor;
			}

			// Add tertiary wave for extreme chaos at very high intensity
			if (chaosFactor > 0.8f) {
				yOffset += sin(time * baseSpeed * 2.2f + colPhase * 3.0f) * 60.0f * chaosFactor;
			}
			
			// Set up the column sprite
			rendTexSp.setTextureRect(colRect);
			rendTexSp.setPosition((float)colX, yOffset);
			rendTexSp.setColor(sf::Color(255, 255, 255, (sf::Uint8)(200 + 55 * (1.0f - chaosFactor))));
			
			// Draw the column
			renderTarget->draw(rendTexSp, activeShader);
		}
		
		// Reset sprite state
		rendTexSp.setTextureRect(sf::IntRect(0, 0, Game::ScreenWidth, Game::ScreenHeight));
		rendTexSp.setPosition(0, 0);
		rendTexSp.setColor(sf::Color::White);
	} else if (doubleVisionEffect > 0.0f) {
		float scale = 1.0f + (doubleVisionEffect * 0.05f);
		rendTexSp.setScale(scale, scale);
		rendTexSp.setOrigin((float)Game::ScreenWidth / 2.0f, (float)Game::ScreenHeight / 2.0f);
		rendTexSp.setPosition((float)Game::ScreenWidth / 2.0f, (float)Game::ScreenHeight / 2.0f);

		// Draw the main image
		renderTarget->draw(rendTexSp, activeShader);

		// Draw the double vision offset image
		float offsetAmount = 10.0f * doubleVisionEffect; // Increased range for more separation
		rendTexSp.setPosition((float)Game::ScreenWidth / 2.0f + offsetAmount, (float)Game::ScreenHeight / 2.0f + offsetAmount * 0.5f);
		rendTexSp.setColor(sf::Color(255, 255, 255, (sf::Uint8)(255 * doubleVisionEffect * 0.5f)));
		renderTarget->draw(rendTexSp, activeShader);

		// Reset for next frame
		rendTexSp.setPosition(0, 0);
		rendTexSp.setOrigin(0, 0);
		rendTexSp.setScale(1.0f, 1.0f);
		rendTexSp.setColor(sf::Color::White);
	} else {
		renderTarget->draw(rendTexSp, activeShader);
	}
// #if DEBUG
// 	sf::Text cornerText("3NgNn3 #4 2021->*now*", *Fonts::MonoFont, 11);
// 	cornerText.setFillColor(sf::Color::Red);
// 	cornerText.setOutlineColor(sf::Color::White);
// 	cornerText.setOutlineThickness(2);
// 	cornerText.setPosition(sf::Vector2f(Game::ScreenWidth - cornerText.getLocalBounds().width, Game::ScreenHeight - cornerText.getLocalBounds().height - 5));
// 	renderTarget->draw(cornerText);
// #endif
#if DEBUG
	{
		std::string debugStr/* = std::format("Coll:{}\n",(int)newCollLineState)*/;
		debugStr += Util::Format("MouseTile:%dx%dy\n", mouseTilePosX, mouseTilePosY);
		debugStr += Util::Format("MouseMapPos:%fx%fy\n", mousePosOnMapX , mousePosOnMapY);
		debugStr += Util::Format("MapName:%s\n", mapFactory->getCurrentMap()->name.c_str());
		debugStr += Util::Format("FPS:%.2f\n", G->framesPerSecond);
		debugStr += Util::Format("RD:%d\n", RainDrops.size());
		debugStr += Util::Format("MTE:%.2fs\n", curMission->missionTimePassed.secs());
		debugStr += Util::Format("BHMS:%.2fms\n", Controller::HeldMillis[(Btn)7]);
		debugStr += Util::Format("PlrAngle:%.2f\n", player->dirAngle);

		debugStr += Util::Format(
			"Is the player on screen?: %s\n"
			//"testpos: %dx %dy\n"		
			,
			plrOnScreen ? "YES" : "no"
			//,scrXwTEST, scrYwTEST
		);
		if (tileEditOn) {
			debugStr += "Tile edit on\n";
			debugStr += Util::Format(
				"Selected layer: %d\n"
				"Selected tile: %d\n"
				,
				selectedLayer,
				tileSelectTileId
			);
		}
		if (moveEntitiesOn) debugStr += "Move entities on\n";
		if (showHitboxes) debugStr += "Hit boxes shown\n";
		if (buildingEditOn) debugStr += "Building edit on\n";
		if (hoveredBuildingWall) debugStr += "Wall is under mouse!\n";
		if (collisionDisabled) debugStr += "Collision disabled\n";
		if (collisionDebug) debugStr += "Collision debug\n";
		if (placingNewNPC) debugStr += "Placing npc...\n";
		if (newCollLineState == DrawNewCollLineState_FirstPoint)
			debugStr += "Coll line: placing first point";
		if (newCollLineState == DrawNewCollLineState_SecondPoint)
			debugStr += "Coll line: placing second point";
		//debugStr += std::format("E ms={}", G->KeysHeldMillis[KB::E]);
		// float computeResult = 0.0f;
		// testComputeShader->runMultiplication(3.0f, 4.0f, computeResult);
		// debugStr += Util::Format("Compute: %.2f\n", computeResult);
		debugText.setString(debugStr);
	}
	// Hide debug text if mouse is over it
	//if (!debugText.getGlobalBounds().contains({ (float)Mouse::Pos_X, (float)Mouse::Pos_Y }))
	{
		renderTarget->draw(debugText);
	}

	// Draw "edit" toggle switch to the left of "open dbg"
	float toggleWidth = 40.0f;
	float toggleHeight = 20.0f;
	float padding = 15.0f;
	
	// Position "open dbg" first to calculate left position for toggle
	sf::Text openDbgText("open dbg", *Fonts::MonoFont, 12);
	openDbgText.setFillColor(sf::Color::Yellow);
	openDbgText.setOutlineColor(sf::Color::Black);
	openDbgText.setOutlineThickness(1.0f);
	auto openDbgBounds = openDbgText.getLocalBounds();
	float openDbgX = Game::ScreenWidth - openDbgBounds.width - 10;
	float openDbgY = Game::ScreenHeight - openDbgBounds.height - 10;
	
	// Position toggle to the left of "open dbg"
	float toggleX = openDbgX - toggleWidth - padding;
	float toggleY = openDbgY + (openDbgBounds.height - toggleHeight) / 2.0f;
	
	// Draw toggle background
	sf::RectangleShape toggleBg(sf::Vector2f(toggleWidth, toggleHeight));
	toggleBg.setPosition(toggleX, toggleY);
	toggleBg.setFillColor(globalEditON ? sf::Color(0, 200, 0, 200) : sf::Color(200, 0, 0, 200));
	toggleBg.setOutlineColor(sf::Color::White);
	toggleBg.setOutlineThickness(1.0f);
	
	// Check if mouse is hovering over toggle
	bool mouseOverToggle = toggleBg.getGlobalBounds().contains({ (float)Mouse::Pos_X, (float)Mouse::Pos_Y });
	if (mouseOverToggle)
	{
		toggleBg.setOutlineColor(sf::Color::Yellow);
		toggleBg.setOutlineThickness(2.0f);
	}
	
	renderTarget->draw(toggleBg);
	
	// Draw toggle label
	sf::Text editLabel("edit", *Fonts::MonoFont, 10);
	editLabel.setFillColor(sf::Color::White);
	editLabel.setOutlineColor(sf::Color::Black);
	editLabel.setOutlineThickness(1.0f);
	auto labelBounds = editLabel.getLocalBounds();
	editLabel.setPosition(toggleX + (toggleWidth - labelBounds.width) / 2.0f, toggleY + (toggleHeight - labelBounds.height) / 2.0f - 2.0f);
	renderTarget->draw(editLabel);
	
	// Handle click on toggle
	if (mouseOverToggle && Mouse::LeftRel && guip_eof == this)
	{
		globalEditON = !globalEditON;
		if (globalEditON)
		{
			Sfx::Open->play(false, 0.3f);
		}
		else
		{
			Sfx::Close->play();
		}
	}
	
	// Draw "open dbg" text in bottom right corner
	openDbgText.setPosition(openDbgX, openDbgY);
	
	// Check if mouse is hovering over "open dbg"
	bool mouseOverOpenDbg = openDbgText.getGlobalBounds().contains({ (float)Mouse::Pos_X, (float)Mouse::Pos_Y });
	if (mouseOverOpenDbg)
	{
		openDbgText.setFillColor(sf::Color::White);
		openDbgText.setOutlineColor(sf::Color::Yellow);
	}
	
	renderTarget->draw(openDbgText);
	
	// Handle click on "open dbg"
	if (mouseOverOpenDbg && Mouse::LeftRel && guip_eof == this)
	{
		debugMenuVisible = !debugMenuVisible;
		if (debugMenuVisible)
		{
			setupDebugMainMenu();
			Sfx::Open->play(false, 0.3f);
		}
		else
		{
			Sfx::Close->play();
		}
	}
#endif
	clockStrTxt.setPosition(sf::Vector2f(Game::ScreenWidth - clockStrTxt.getLocalBounds().width - 10, 10));
	renderTarget->draw(clockStrTxt);

	if (showInventoryPopup) {
		invPopup->draw(renderTarget, player);
	}
	
	// Draw fuel gauge if player is in a car
	if (player->carImInsideOf != nullptr) {
		float gaugeCenterX = 50.0f;
		float gaugeCenterY = Game::ScreenHeight - 120.0f;
		float gaugeRadius = 30.0f;

		// Draw gauge background (circular)
		sf::CircleShape gaugeBg(gaugeRadius);
		gaugeBg.setPosition(gaugeCenterX - gaugeRadius, gaugeCenterY - gaugeRadius);
		gaugeBg.setFillColor(sf::Color(30, 30, 30, 200));
		gaugeBg.setOutlineThickness(3.0f);
		gaugeBg.setOutlineColor(sf::Color::White);
		renderTarget->draw(gaugeBg);

		// Draw E and F labels
		sf::Text eText("E", *Fonts::OSDFont, 16);
		eText.setFillColor(sf::Color::Black);
		eText.setOutlineColor(sf::Color::White);
		eText.setOutlineThickness(2.0f);
		float eX = gaugeCenterX - gaugeRadius * 0.85f;
		float eY = gaugeCenterY + gaugeRadius * 0.4f;
		eText.setPosition(eX, eY);
		renderTarget->draw(eText);

		sf::Text fText("F", *Fonts::OSDFont, 16);
		fText.setFillColor(sf::Color::Black);
		fText.setOutlineColor(sf::Color::White);
		fText.setOutlineThickness(2.0f);
		float fX = gaugeCenterX + gaugeRadius * 0.65f;
		float fY = gaugeCenterY + gaugeRadius * 0.4f;
		fText.setPosition(fX, fY);
		renderTarget->draw(fText);

		// Draw needle
		float fuelPercent = player->carImInsideOf->fuelLevel / 100.0f;
		// F is at ~40° (bottom-right), E is at ~140° (bottom-left)
		// Sweeping CCW through the top arc means going from 40° down to -220° (which is 140°)
		float needleAngle = -220.0f + (fuelPercent * 260.0f);

		float needleLength = gaugeRadius * 0.8f;
		float needleEndX = gaugeCenterX + needleLength * cos(Util::ToRad(needleAngle));
		float needleEndY = gaugeCenterY + needleLength * sin(Util::ToRad(needleAngle));

		sf::VertexArray needle(sf::PrimitiveType::Lines, 2);
		needle[0].position = sf::Vector2f(gaugeCenterX, gaugeCenterY);
		needle[0].color = sf::Color::Red;
		needle[1].position = sf::Vector2f(needleEndX, needleEndY);
		needle[1].color = sf::Color::Red;
		renderTarget->draw(needle);

		// Draw center dot
		sf::CircleShape centerDot(3.0f);
		centerDot.setPosition(gaugeCenterX - 3.0f, gaugeCenterY - 3.0f);
		centerDot.setFillColor(sf::Color::Red);
		renderTarget->draw(centerDot);

		// Draw gear indicator
		std::string gearStr = player->carImInsideOf->isGearReversing ? "R" : "D";
		if (player->carImInsideOf->gearSwitchTimer > 0.0f) gearStr = "-"; // Show "-" during shift
		
		sf::Text gearText(gearStr, *Fonts::OSDFont, 28);
		gearText.setStyle(sf::Text::Bold);
		if (gearStr == "-") {
			// Blink during gear shift
			if ((int)(player->carImInsideOf->gearSwitchTimer * 10) % 2 == 0)
				gearText.setFillColor(sf::Color(255, 255, 255, 100));
			else
				gearText.setFillColor(sf::Color(255, 255, 255, 255));
		} else {
			gearText.setFillColor(sf::Color::White);
		}
		
		gearText.setOutlineColor(sf::Color::Black);
		gearText.setOutlineThickness(2.0f);
		
		// Position it above the fuel gauge
		sf::FloatRect gearBounds = gearText.getLocalBounds();
		gearText.setOrigin(gearBounds.width / 2.0f, gearBounds.height / 2.0f);
		gearText.setPosition(gaugeCenterX, gaugeCenterY - gaugeRadius - 35.0f);
		renderTarget->draw(gearText);
		
		// Gear label
		sf::Text gearLabel("GEAR", *Fonts::OSDFont, 12);
		gearLabel.setFillColor(sf::Color(200, 200, 200));
		sf::FloatRect labelBounds = gearLabel.getLocalBounds();
		gearLabel.setOrigin(labelBounds.width / 2.0f, labelBounds.height / 2.0f);
		gearLabel.setPosition(gaugeCenterX, gaugeCenterY - gaugeRadius - 55.0f);
		renderTarget->draw(gearLabel);
	}

	// Update and render minimap
	minimap->update(mapFactory->getCurrentMap(), player, playerAutoWalking, playerAutoWalkToPosX, playerAutoWalkToPosY, mousePosOnMapX, mousePosOnMapY);
	if (G->curScreenFramesPassed == 0) {
		minimap->startSlideInAnimation();
	}
	minimap->render(*renderTarget);

	// Check for minimap click to open MapScreen
	if ( (Mouse::LeftRel && guip_eof == (GuiWidget*)minimap) || Kb::IsKeyFirstFrame(KB::M) )
	{
		if (MapScreen::Instance) {
			MapScreen::Instance->switchTo();
		}
	}

	//for(auto& e : player->hostMap->entities)
	//{
	//	if (e == player) continue;
	//	if (auto a = dynamic_cast<Actor*>(&*e)) {
	//		if (a->health < Actor::HealthEnergyBasedMax) {
	//			auto hbPos = mapPosToScreenPos(
	//	sf::Vector2f(a->bodyRect.left, a->bodyRect.top));
	//			//a->headHealthBar.render(sf::Vector2f(hbPos.x, hbPos.y));
	//		}
	//	}
	//}

	dbox.update();
	dbox.render(renderTarget);

	playerHealthBar->update();
	playerEnergyBar->update();
	playerHungerBar->update();
	playerBladderBar->update();
	playerBasedMeterBar->update();

	int hbsv = 0;
	playerHealthBar->render(15, (++hbsv) * 15, renderTarget);
	playerEnergyBar->render(15, (++hbsv) * 15, renderTarget);
	playerHungerBar->render(15, (++hbsv) * 15, renderTarget);
	playerBladderBar->render(15, (++hbsv) * 15, renderTarget);
	playerBasedMeterBar->render(15, (++hbsv) * 15, renderTarget);

	// Update drug effect values from renderer and render drug effect health bars
	if (New3DRenderer* renderer3D = dynamic_cast<New3DRenderer*>(renderer)) {
		wavyWorldEffect = renderer3D->getCurrentWavyWorldEffect() * 100.0f;
		worldVibrationEffect = renderer3D->getCurrentWorldVibrationEffect() * 100.0f;
		colorShiftEffect = renderer3D->getCurrentColorShiftEffect() * 100.0f;
		kaleidoscopeEffect = renderer3D->getCurrentKaleidoscopeEffect() * 100.0f;
		noiseEffect = renderer3D->getCurrentNoiseEffect() * 100.0f;
		greyscaleEffect = renderer3D->getCurrentGreyscaleEffect() * 100.0f;
		stretchEffect = renderer3D->getCurrentStretchEffect() * 100.0f;
		bitcrushEffect = renderer3D->getCurrentBitcrushEffect() * 100.0f;
		worldScramblerEffect = renderer3D->getCurrentWorldScramblerEffect() * 100.0f;
		fractalEffect = renderer3D->getCurrentFractalEffect() * 100.0f;
		depthPerceptionEffect = renderer3D->getCurrentDepthPerceptionEffect() * 100.0f;
		dissolveEffect = renderer3D->getCurrentDissolveEffect() * 100.0f;
		rgbDeconvergenceEffect = renderer3D->getCurrentRGBDeconvergenceEffect() * 100.0f;
		radialBlurEffect = renderer3D->getCurrentRadialBlurEffect() * 100.0f;
		bloomEffect = renderer3D->getCurrentBloomEffect() * 100.0f;
#if DEBUG
		// Calculate max effect value for softmax-style scaling
		float maxEffect = 0.0f;
		maxEffect = fmaxf(maxEffect, wavyWorldEffect);
		maxEffect = fmaxf(maxEffect, worldVibrationEffect);
		maxEffect = fmaxf(maxEffect, colorShiftEffect);
		maxEffect = fmaxf(maxEffect, kaleidoscopeEffect);
		maxEffect = fmaxf(maxEffect, noiseEffect);
		maxEffect = fmaxf(maxEffect, greyscaleEffect);
		maxEffect = fmaxf(maxEffect, stretchEffect);
		maxEffect = fmaxf(maxEffect, bitcrushEffect);
		maxEffect = fmaxf(maxEffect, worldScramblerEffect);
		maxEffect = fmaxf(maxEffect, fractalEffect);
		maxEffect = fmaxf(maxEffect, depthPerceptionEffect);
		maxEffect = fmaxf(maxEffect, dissolveEffect);
		maxEffect = fmaxf(maxEffect, rgbDeconvergenceEffect);
		maxEffect = fmaxf(maxEffect, radialBlurEffect);
		maxEffect = fmaxf(maxEffect, bloomEffect);

		// Count active effects to determine scaling strategy
		int activeEffectCount = 0;
		if (wavyWorldEffect > 0.0f) activeEffectCount++;
		if (worldVibrationEffect > 0.0f) activeEffectCount++;
		if (colorShiftEffect > 0.0f) activeEffectCount++;
		if (kaleidoscopeEffect > 0.0f) activeEffectCount++;
		if (noiseEffect > 0.0f) activeEffectCount++;
		if (greyscaleEffect > 0.0f) activeEffectCount++;
		if (stretchEffect > 0.0f) activeEffectCount++;
		if (bitcrushEffect > 0.0f) activeEffectCount++;
		if (worldScramblerEffect > 0.0f) activeEffectCount++;
		if (fractalEffect > 0.0f) activeEffectCount++;
		if (depthPerceptionEffect > 0.0f) activeEffectCount++;
		if (dissolveEffect > 0.0f) activeEffectCount++;
		if (rgbDeconvergenceEffect > 0.0f) activeEffectCount++;
		if (radialBlurEffect > 0.0f) activeEffectCount++;
		if (bloomEffect > 0.0f) activeEffectCount++;

		// Apply softmax-style scaling only when multiple effects are active
		// Use a minimum threshold to avoid division by zero and excessive scaling of tiny values
		float scaleMax = fmaxf(maxEffect, 10.0f); // At least 10% max to prevent over-scaling
		
		if (maxEffect > 0.0f && activeEffectCount > 1) {
			// Softmax-style: scale relative to max so highest fills the bar
			// The highest value will be at 100, others scaled relative to it
			float scaleFactor = 100.0f / scaleMax;
			
			wavyWorldEffectDisplay = wavyWorldEffect * scaleFactor;
			worldVibrationEffectDisplay = worldVibrationEffect * scaleFactor;
			colorShiftEffectDisplay = colorShiftEffect * scaleFactor;
			kaleidoscopeEffectDisplay = kaleidoscopeEffect * scaleFactor;
			noiseEffectDisplay = noiseEffect * scaleFactor;
			greyscaleEffectDisplay = greyscaleEffect * scaleFactor;
			stretchEffectDisplay = stretchEffect * scaleFactor;
			bitcrushEffectDisplay = bitcrushEffect * scaleFactor;
			worldScramblerEffectDisplay = worldScramblerEffect * scaleFactor;
			fractalEffectDisplay = fractalEffect * scaleFactor;
			depthPerceptionEffectDisplay = depthPerceptionEffect * scaleFactor;
			dissolveEffectDisplay = dissolveEffect * scaleFactor;
			rgbDeconvergenceEffectDisplay = rgbDeconvergenceEffect * scaleFactor;
			radialBlurEffectDisplay = radialBlurEffect * scaleFactor;
			bloomEffectDisplay = bloomEffect * scaleFactor;
		} else {
			// Only one effect active (or none), show real values
			wavyWorldEffectDisplay = wavyWorldEffect;
			worldVibrationEffectDisplay = worldVibrationEffect;
			colorShiftEffectDisplay = colorShiftEffect;
			kaleidoscopeEffectDisplay = kaleidoscopeEffect;
			noiseEffectDisplay = noiseEffect;
			greyscaleEffectDisplay = greyscaleEffect;
			stretchEffectDisplay = stretchEffect;
			bitcrushEffectDisplay = bitcrushEffect;
			worldScramblerEffectDisplay = worldScramblerEffect;
			fractalEffectDisplay = fractalEffect;
			depthPerceptionEffectDisplay = depthPerceptionEffect;
			dissolveEffectDisplay = dissolveEffect;
			rgbDeconvergenceEffectDisplay = rgbDeconvergenceEffect;
			radialBlurEffectDisplay = radialBlurEffect;
			bloomEffectDisplay = bloomEffect;
		}
#endif
		// Update all drug effect health bars
		wavyWorldDrugBar->update();
		worldVibrationDrugBar->update();
		colorShiftDrugBar->update();
		kaleidoscopeDrugBar->update();
		noiseDrugBar->update();
		greyscaleDrugBar->update();
		stretchDrugBar->update();
		bitcrushDrugBar->update();
		worldScramblerDrugBar->update();
		fractalDrugBar->update();
		depthPerceptionDrugBar->update();
		dissolveDrugBar->update();
		rgbDeconvergenceDrugBar->update();
		radialBlurDrugBar->update();
		bloomDrugBar->update();

		// Render drug effect health bars only when active (> 0.0)
		if (wavyWorldEffect > 0.0f) wavyWorldDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (worldVibrationEffect > 0.0f) worldVibrationDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (colorShiftEffect > 0.0f) colorShiftDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (kaleidoscopeEffect > 0.0f) kaleidoscopeDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (noiseEffect > 0.0f) noiseDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (greyscaleEffect > 0.0f) greyscaleDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (stretchEffect > 0.0f) stretchDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (bitcrushEffect > 0.0f) bitcrushDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (worldScramblerEffect > 0.0f) worldScramblerDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (fractalEffect > 0.0f) fractalDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (depthPerceptionEffect > 0.0f) depthPerceptionDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (dissolveEffect > 0.0f) dissolveDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (rgbDeconvergenceEffect > 0.0f) rgbDeconvergenceDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (radialBlurEffect > 0.0f) radialBlurDrugBar->render(15, (++hbsv) * 15, renderTarget);
		if (bloomEffect > 0.0f) bloomDrugBar->render(15, (++hbsv) * 15, renderTarget);
	}

	if (
		(Kb::IsKeyFirstFrame(KB::Escape) || Controller::BtnFrames[Btn_Start] == 1)
		&& G->curScreenFramesPassed > 30
	)
	{
		Sfx::Close->play();
		PauseScreen::Instance->switchTo();
	}

	//interactables.push_back(openInvInter);
	if(HotPocket* hp = dynamic_cast<HotPocket*>(player->inv->equippedItem))
    {
        if(hp->cooked)
        {
            interactables.push_back(hp->eatInteraction);
        }
    }
#if DEBUG
	if (guip_eof == this)
	{
		if (Kb::IsKeyReleased(KB::C))
		{
			initialCam = true;
			curMission->camSettled = false;
			Sfx::Close->play();
		}

		if (Kb::IsKeyReleased(KB::H))
		{
			player->healCompletely();
			Sfx::Open->play(false, 0.3f);
		}

		if (Kb::IsKeyReleased(KB::F1))
		{
			showDebugMoveCircle = !showDebugMoveCircle;
			if (showDebugMoveCircle)
			{
				Sfx::Open->play(false, 0.3f);
			}
			else
			{
				Sfx::Close->play();
			}
		}

		if (Kb::IsKeyDown(KB::LControl)
			&& Kb::IsKeyReleased(KB::W)
		)
		{
			player->emptyBladder(nullptr);
			Sfx::Open->play(false, 0.3f);
		}

		if (Kb::IsKeyDown(KB::LControl)
			&& Kb::IsKeyDown(KB::Return)
		)
		{
			if (Kb::IsKeyReleased(KB::Num1))
			{
				IGS->tileEditOn = !IGS->tileEditOn;
				if (IGS->tileEditOn)
				{
					Sfx::Open->play(false, 0.3f);
				}
				else
				{
					Sfx::Close->play();
				}
			}

			if (Kb::IsKeyReleased(KB::Num2))
			{
				IGS->collisionDisabled = !IGS->collisionDisabled;
				if (IGS->collisionDisabled)
				{
					Sfx::Open->play(false, 0.3f);
				}
				else
				{
					Sfx::Close->play();
				}
			}

			if (Kb::IsKeyReleased(KB::Num3))
			{
				IGS->collisionDebug = !IGS->collisionDebug;
				if (IGS->collisionDebug)
				{
					Sfx::Open->play(false, 0.3f);
				}
				else
				{
					Sfx::Close->play();
				}
			}

			if (Kb::IsKeyReleased(KB::Num4))
			{
				IGS->showHitboxes = !IGS->showHitboxes;
				if (IGS->showHitboxes)
				{
					Sfx::Open->play(false, 0.3f);
				}
				else
				{
					Sfx::Close->play();
				}
			}

			if (Kb::IsKeyReleased(KB::Num5))
			{
				IGS->placingNewNPC = !IGS->placingNewNPC;
				if (IGS->placingNewNPC)
				{
					Sfx::Open->play(false, 0.3f);
				}
				else
				{
					Sfx::Close->play();
				}
			}
			else if (Kb::IsKeyReleased(KB::Num6))
			{
				newCollLineState = DrawNewCollLineState_FirstPoint;
				cm->collLines.push_back(new CollisionLine(IGS->mousePosOnMapX, IGS->mousePosOnMapY, IGS->mousePosOnMapX, IGS->mousePosOnMapY));
				IGS->collisionDebug = true;
				selectedCollisionLineIndex = cm->collLines.size() - 1;
				Sfx::Open->play(false, 0.3f);
			}
			else if (Kb::IsKeyReleased(KB::Num7))
			{
				IGS->player->bladder = 100;
				Sfx::Open->play(false, 0.3f);
			}
			else if (Kb::IsKeyReleased(KB::Num8))
			{
				IGS->raining = !IGS->raining;
				if (IGS->raining)
				{
					Sfx::Open->play(false, 0.3f);
				}
				else
				{
					Sfx::Close->play();
					RainDrops.clear();
				}
			}
			else if (Kb::IsKeyReleased(KB::Num9))
			{
				IGS->showEntityOBBDebug = !IGS->showEntityOBBDebug;
				if (IGS->showEntityOBBDebug)
				{
					Sfx::Open->play(false, 0.3f);
				}
				else
				{
					Sfx::Close->play();
				}
			}
			else if (Kb::IsKeyReleased(KB::Num0))
			{
				debugMenuVisible = !debugMenuVisible;
				if (debugMenuVisible)
				{
					// Reset to main menu when opening
					setupDebugMainMenu();
					Sfx::Open->play(false, 0.3f);
				}
				else
				{
					Sfx::Close->play();
				}
			}
		}
	}
	if (placingNewNPC)
	{
		guip = (GuiWidget*) &placingNewNPC;
		if (Mouse::LeftRel
			&& guip_eof == (GuiWidget*) &placingNewNPC
		)
		{
			auto cn = new Catgirl();
			cn->setPosTile(mouseTilePosX, mouseTilePosY);
			/*auto gun = new Gun();
			cn->inv.equippedItem = &*gun;
			cn->inv.addItem(gun);*/
			cn->follow = player;
			player->hostMap->addEnt(cn);
			placingNewNPC = false;
		}
	}
	if (showDebugMoveCircle)
	{
		sf::CircleShape cs;
		sf::Vector2f mid(Game::ScreenWidth / 2, Game::ScreenHeight / 2);
		cs.setOrigin(sf::Vector2f(moveDist / zoom, moveDist / zoom));
		cs.setPosition(mid);
		cs.setRadius(moveDist / zoom);
		cs.setOutlineThickness(3);
		cs.setFillColor(sf::Color::Transparent);
		cs.setOutlineColor(sf::Color(0, 255, 0, 80));
		renderTarget->draw(cs);
	}

	// interactables.push_back(newPortalInteraction);
	interactables.push_back(spawnCarInter);
	//interactables.push_back(giveWeaponsInter);

	// Update and render debug menu if visible
	if (debugMenuVisible && debugMenuPrompt)
	{
		// Handle Back/Escape key for navigation
		if (Kb::IsKeyFirstFrame(KB::Escape) || Kb::IsKeyFirstFrame(KB::Backspace))
		{
			if (currentDebugMenuLevel != DebugMenu_Main)
			{
				// Go back to main menu
				setupDebugMainMenu();
				Sfx::Close->play();
			}
			else
			{
				// Close menu from main screen
				debugMenuVisible = false;
				Sfx::Close->play();
			}
		}
		
		debugMenuPrompt->update();
		debugMenuPrompt->render(renderTarget);
	}
#endif
	auto intssize = interactables.size();
	bool intActivated = false;
	//Interaction* highest = nullptr;
	int highestPriority = INT_MIN;
	int highestIndex = 0;

	{
		int idx = 0;
		for(auto i : interactables)
		{
			if(i->priorityLevel > highestPriority)
			{
				//highest = i;
				highestIndex = idx;
				highestPriority = i->priorityLevel;
			}
			idx++;
		}
	}
	
	if((player->flags & Entity::EntFlag_Animating) && highestPriority > 0)
	{
		interactableIndex = highestIndex;
	}
	
	if (intssize > 0
		&& !dbox.showing
		&& player->isControllable
		&& !(player->flags & Entity::EntFlag_Dead)
		&& G->winFocused
		&& !showInventoryPopup
		)
	{
		/* cycle interactables*/
		if (Kb::IsKeyFirstFrame(KB::Tab) || Controller::BtnFrames[Btn_RBtn] == 1)
		{
			if (Kb::IsKeyDown(KB::LShift))
			{
				if (interactableIndex == 0)
				{
					interactableIndex = intssize - 1;
				}
				else
				{
					--interactableIndex;
				}
			}
			else {
				if (interactableIndex == intssize - 1)
				{
					interactableIndex = 0; // cycle back
				}
				else
				{
					++interactableIndex;
				}
			}
		}
		if(Controller::BtnFrames[Btn_LBtn] == 1)
		{			
			if (interactableIndex == 0)
			{
				interactableIndex = intssize - 1;
			}
			else
			{
				--interactableIndex;
			}
		}
		interactableIndex = Util::Clamp(interactableIndex, (size_t)0, intssize - 1);
		if (intssize < 1)
		{
			interactableIndex = 0;
			if (!interactableText.getString().isEmpty())
			{
				interactableText.setString("");
			}
		}
		else
		{
			interactableText.setString(Util::Format("%sPress %s to %s",
				intssize > 1 ? Util::Format("[Tab to cycle] (%d/%d)\n",
					interactableIndex + 1, intssize).c_str() : "",
				G->inMethod == InputMethod_Keyboard ?
				"E" :
				G->inMethod == InputMethod_Controller ?
				"Square" :
				"",
				interactables.at(interactableIndex)->name.c_str()));
		}
		/* activate interactable(s)*/
		auto& in = interactables.at(interactableIndex);
		auto activateInter = Kb::IsKeyReleased(KB::E) || Controller::BtnRel[Btn_Square];
		auto eff = activateInter && in->requireHoldMs == 0;
		auto khmsE = G->inMethod == InputMethod_Keyboard ? Kb::GetKeyMs(KB::E) :
			G->inMethod == InputMethod_Controller ? Controller::HeldMillis[Btn_Square] : 0;
		if (!intActivated && (eff || (in->requireHoldMs > 0 && khmsE >= in->requireHoldMs && activateInter)))
		{
			in->call(player);
			curMission->onEntityInteract(in->e);
			Sfx::Open->play(false, 0.3f);
			Kb::ResetKeyMs(KB::E);
			intActivated = true;
		}
		/*draw text saying what the interactable is */
		auto bounds = interactableText.getLocalBounds();
		auto str = interactableText.getString().toAnsiString();
		interactableText.setOrigin(sf::Vector2f(bounds.width / 2, bounds.height / 2));
		sf::Vector2f itp(Game::ScreenWidth / 2, Game::ScreenHeight * 0.75f);
		interactableText.setPosition(itp);

		// Draw interaction buttons
		if (intssize >= 1) {
			float buttonY = Game::ScreenHeight - 50;
			float buttonSpacing = 80;
			float totalWidth = buttonSpacing * 2;
			float startX = Game::ScreenWidth / 2 - totalWidth / 2;
			
			// Previous button
			RectangleButton prevBtn(30, 30);
			prevBtn.shape.setPosition(startX, buttonY);
			prevBtn.shape.setFillColor(sf::Color(100, 100, 100, 150));
			prevBtn.shape.setOutlineThickness(2);
			prevBtn.shape.setOutlineColor(sf::Color::White);
			if(intssize > 1) renderTarget->draw(prevBtn.shape);
			
			sf::Text prevText("<", *Fonts::MainFont, 20);
			prevText.setPosition(startX + 10, buttonY + 5);
			prevText.setFillColor(sf::Color::White);
			if(intssize > 1) renderTarget->draw(prevText);
			
			// Next button
			RectangleButton nextBtn(30, 30);
			nextBtn.shape.setPosition(startX + buttonSpacing, buttonY);
			nextBtn.shape.setFillColor(sf::Color(100, 100, 100, 150));
			nextBtn.shape.setOutlineThickness(2);
			nextBtn.shape.setOutlineColor(sf::Color::White);
			if(intssize > 1) renderTarget->draw(nextBtn.shape);
			
			sf::Text nextText(">", *Fonts::MainFont, 20);
			nextText.setPosition(startX + buttonSpacing + 10, buttonY + 5);
			nextText.setFillColor(sf::Color::White);
			if(intssize > 1) renderTarget->draw(nextText);
			
			// Activate button
			RectangleButton activateBtn(60, 30);
			activateBtn.shape.setPosition(startX + buttonSpacing * 2, buttonY);
			activateBtn.shape.setFillColor(sf::Color(0, 150, 0, 150));
			activateBtn.shape.setOutlineThickness(2);
			activateBtn.shape.setOutlineColor(sf::Color::Green);
			renderTarget->draw(activateBtn.shape);
			
			sf::Text activateText("USE", *Fonts::MainFont, 16);
			activateText.setPosition(startX + buttonSpacing * 2 + 15, buttonY + 7);
			activateText.setFillColor(sf::Color::White);
			renderTarget->draw(activateText);
			
			// Check hover and set guip
			sf::Vector2i mousePos = sf::Vector2i{ Mouse::Pos_X, Mouse::Pos_Y };
			if (intssize > 1 && prevBtn.shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
				guip = &prevBtn;
			} else if (intssize > 1 && nextBtn.shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
				guip = &nextBtn;
			} else if (activateBtn.shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
				guip = &activateBtn;
			}
			
			// Handle button clicks with proper GUI focus check
			if (Mouse::LeftFrames == 1 && guip_eof) {
				// Check previous button
				if (guip_eof == &prevBtn) {
					if (interactableIndex == 0) {
						interactableIndex = intssize - 1;
					} else {
						--interactableIndex;
					}
					Sfx::CursorMove->play();
				}
				// Check next button
				else if (guip_eof == &nextBtn) {
					if (interactableIndex == intssize - 1) {
						interactableIndex = 0;
					} else {
						++interactableIndex;
					}
					Sfx::CursorMove->play();
				}
				// Check activate button
				else if (guip_eof == &activateBtn) {
					auto& in = interactables.at(interactableIndex);
					if (!intActivated) {
						in->call(player);
						curMission->onEntityInteract(in->e);
						Sfx::Open->play(false, 0.3f);
						intActivated = true;
					}
				}
			}
		}

		renderTarget->draw(interactableText);

		if (in->requireHoldMs > 0 && khmsE > 0) {
			float barW = 150;
			sf::RectangleShape rmsBar;
			rmsBar.setOutlineColor(
				sf::Color(0, 0, 0, Util::ScaleClamped(khmsE, 0, in->requireHoldMs, 80, 255)));
			rmsBar.setFillColor(
				sf::Color(
					255 - Util::ScaleClamped(khmsE, 0, in->requireHoldMs, 0, 255),
					Util::ScaleClamped(khmsE, 0, in->requireHoldMs, 0, 255),
					0,
					Util::ScaleClamped(khmsE, 0, in->requireHoldMs, 80, 175)
				)
			);
			rmsBar.setOutlineThickness(Util::ScaleClamped(khmsE, 0, in->requireHoldMs, 0, 3));
			rmsBar.setPosition(sf::Vector2f(Game::ScreenWidth / 2, itp.y + bounds.height + 10));
			rmsBar.setOrigin(barW / 2, 10);
			auto sc = Util::ScaleClamped(khmsE, 0, in->requireHoldMs, 0.89, 1.1);
			rmsBar.setScale(sc, sc);
			rmsBar.setSize(sf::Vector2f(Util::ScaleClamped(khmsE, 0, in->requireHoldMs, 0,barW), 20));
			renderTarget->draw(rmsBar);
		}
	}

	if (showInventoryPopup) {
		invPopup->handleInput(player);
	}

	if (player->canUseItems())
	{
		sf::Text useText(std::string("Click to use ") + player->inv->equippedItem->name, *Fonts::MainFont, 24);
		useText.setPosition(sf::Vector2f(5, Game::ScreenHeight - 25));
		useText.setOrigin(0, useText.getGlobalBounds().height);
		useText.setFillColor(sf::Color::White);
		renderTarget->draw(useText);
	}

	interactables.clear();

#if DEBUG
	// When global edit mode is on, check for nearby building walls and add toggle interaction
	if (globalEditON)
	{
		BuildingWall* nearestWall = nullptr;
		float nearestDist = 100.0f; // Detection radius
		Map* cm = mapFactory->getCurrentMap();
		
		for (Building* b : cm->buildings)
		{
			for (Floor* f : b->floors)
			{
				for (BuildingWall& w : f->walls)
				{
					// Calculate distance from player to wall
					float wallCenterX = (w.x1 + w.x2) / 2.0f + b->posX;
					float wallCenterY = (w.y1 + w.y2) / 2.0f + b->posY;
					float dist = Util::Dist((float)player->posX, (float)player->posY, wallCenterX, wallCenterY);
					
					if (dist < nearestDist)
					{
						nearestDist = dist;
						nearestWall = &w;
					}
				}
			}
		}
		
		if (nearestWall)
		{
			hoveredBuildingWall = nearestWall;
			interactables.push_back(toggleWallDestructableInter);
		}
		else
		{
			hoveredBuildingWall = nullptr;
		}
	}
#endif

	// Update and render radio station display
	if (radioDisplay.active) {
		radioDisplay.displayTime += G->frameDeltaMillis / 1000.0f;
		if (radioDisplay.displayTime > 3.0f) {
			// Fade out over 1 second
			radioDisplay.alpha = Util::Clamp(1.0f - (radioDisplay.displayTime - 3.0f), 0.0f, 1.0f);
			if (radioDisplay.alpha <= 0.0f) {
				radioDisplay.active = false;
				radioDisplay.displayTime = 0.0f;
				radioDisplay.alpha = 1.0f;
			}
		}
		
		if (radioDisplay.active) {
			sf::Text radioText(radioDisplay.stationName, *Fonts::MainFont, 24);
			radioText.setFillColor(sf::Color(255, 255, 255, (sf::Uint8)(radioDisplay.alpha * 255)));
			radioText.setOutlineColor(sf::Color::Black);
			radioText.setOutlineThickness(2.0f);
			
			// Position above minimap
			float minimapX = minimap->getPositionX();
			float minimapY = minimap->getPositionY();
			float minimapSizeX = minimap->getSizeX();
			float minimapSizeY = minimap->getSizeY();
			
			sf::Vector2f textPos(minimapX + minimapSizeX / 2, minimapY - 40);
			radioText.setOrigin(radioText.getLocalBounds().width / 2, radioText.getLocalBounds().height);
			radioText.setPosition(textPos);
			
			renderTarget->draw(radioText);
		}
	}

	// Update and render temporary message
	if (tempMessage.active) {
		tempMessage.displayTime += G->frameDeltaMillis / 1000.0f;
		if (tempMessage.displayTime > tempMessage.duration) {
			// Fade out over 0.5 seconds
			float fadeProgress = (tempMessage.displayTime - tempMessage.duration) / 0.5f;
			if (fadeProgress >= 1.0f) {
				tempMessage.active = false;
			}
		}
		
		if (tempMessage.active) {
			sf::Text msgText(tempMessage.message, *Fonts::MainFont, 28);
			sf::Color textColor = tempMessage.color;
			// Apply fade out
			if (tempMessage.displayTime > tempMessage.duration) {
				float fadeProgress = (tempMessage.displayTime - tempMessage.duration) / 0.5f;
				textColor.a = (sf::Uint8)(255 * (1.0f - fadeProgress));
			}
			msgText.setFillColor(textColor);
			msgText.setOutlineColor(sf::Color::Black);
			msgText.setOutlineThickness(2.0f);
			
			// Center of screen
			auto bounds = msgText.getLocalBounds();
			msgText.setOrigin(bounds.width / 2, bounds.height / 2);
			msgText.setPosition(Game::ScreenWidth / 2, Game::ScreenHeight / 2);
			
			renderTarget->draw(msgText);
		}
	}

	if (missionFailedTextShown && !(player->flags & Entity::EntFlag_Dead)) {
		missionFailedTextScaleTimer.update();
		sf::Text mft(Util::Format("Mission failed\nin %.2f seconds!!!",
			curMission->missionTimePassed.secs()), *Fonts::OSDFont, 40);
		mft.setPosition(Game::ScreenWidth / 2,Game::ScreenHeight / 2);
		auto lb = mft.getLocalBounds();
		float sc = Util::ScaleClamped(missionFailedTextScaleTimer.millis(), 0,
			missionFailedTextScaleTimer.lengthMs, 0.001f, 1.f);
		mft.setScale(sc, sc);
		mft.setOrigin(lb.width / 2, lb.height / 2);
		mft.setFillColor(sf::Color::Red);
		mft.setOutlineColor(sf::Color::Black);
		mft.setOutlineThickness(2.3);
		renderTarget->draw(mft);
	}

	if (player->flags & Entity::EntFlag_Dead)
	{
		sf::Text pdt(std::string("") + (missionFailedTextShown ? "killed in action.." : "you died"), *Fonts::OSDFont, 50);
		auto lb = pdt.getLocalBounds();
		pdt.setOrigin(lb.width / 2, lb.height);
		pdt.setPosition(Game::ScreenWidth / 2, Game::ScreenHeight / 2);
		pdt.setFillColor(sf::Color::Red);
		pdt.setOutlineColor(sf::Color::Black);
		pdt.setOutlineThickness(2.3);
		renderTarget->draw(pdt);
		deathPrompt->update();
		deathPrompt->render(renderTarget);
	}
#if DEBUG
	bool angleSnapOn = Kb::IsKeyDown(KB::LShift);
	bool tileAlignmentOff = Kb::IsKeyDown(KB::LControl);
	if (tileAlignmentOff)
	{
		newPointPosX = mousePosOnMapX;
		newPointPosY = mousePosOnMapY;
	}
	else
	{
		newPointPosX = mouseTilePosMidClosestX * Map::TileSizePixels;
		newPointPosY = mouseTilePosMidClosestY * Map::TileSizePixels;
	}

	if (cm->collLines.size() > 0)
	{
		selectedCollisionLineIndex
			= Util::Clamp(selectedCollisionLineIndex, 0, cm->collLines.size() - 1);
		auto cl = cm->collLines[selectedCollisionLineIndex];
		if (newCollLineState == DrawNewCollLineState_FirstPoint)
		{
			guip = (GuiWidget*) cl;
			cl->p1 = { newPointPosX, newPointPosY };
			cl->p2 = { newPointPosX, newPointPosY };
			if (Mouse::LeftRel && guip_eof == (GuiWidget*) cl)
			{
				Sfx::Pick->play();
				newCollLineState = DrawNewCollLineState_SecondPoint;
			}
		}
		else if (newCollLineState == DrawNewCollLineState_SecondPoint)
		{
			guip = (GuiWidget*) cl;
			float mouseAngle = Util::RotateTowards(cl->p1.x, cl->p1.y, newPointPosX, newPointPosY);
			int angleSnap = 10;
			mouseAngle = Util::Round(mouseAngle / angleSnap) * angleSnap;
			Point mousePosAngleLocked{};
			Util::AngleLineRel(cl->p1.x, cl->p1.y, mouseAngle, Util::Dist(cl->p1.x, cl->p1.y, newPointPosX, newPointPosY), &mousePosAngleLocked.x, &mousePosAngleLocked.y);
			sf::Vector2f asonp = angleSnapOn ? sf::Vector2f(static_cast<float>(mousePosAngleLocked.x), static_cast<float>(mousePosAngleLocked.y)) : sf::Vector2f{ static_cast<float>(newPointPosX), static_cast<float>(newPointPosY) };
			cl->p2.x = asonp.x;
			cl->p2.y = asonp.y;
			if (sf::Vector2f{ static_cast<float>(cl->p1.x), static_cast<float>(cl->p1.y) } == sf::Vector2f{ static_cast<float>(cl->p2.x), static_cast<float>(cl->p2.y) } && (Mouse::LeftRel || Mouse::RightRel))
			{
				auto l = cm->collLines.begin();
				std::advance(l, selectedCollisionLineIndex);
				cm->collLines.erase(l);
				selectedCollisionLineIndex = 0;
				newCollLineState = DrawNewCollLineState_Done;
				Sfx::Pick->play();
			}
			else if (newCollLineState != DrawNewCollLineState_Done && Mouse::RightRel)
			{
				newCollLineState = DrawNewCollLineState_Done;
				cm->needPathfindingNodesRecalc = true;
				Sfx::Pick->play();
			}
			else if (Mouse::LeftRel && guip_eof == (GuiWidget*) cl)
			{
				newCollLineState = DrawNewCollLineState_SecondPoint;
				auto cl2 = cm->collLines.begin();
				std::advance(cl2, cm->collLines.size() - 1);
				sf::Vector2f lcpp = cm->collLines.size() > 0 ? sf::Vector2f{static_cast<float>((*cl2)->p2.x), static_cast<float>((*cl2)->p2.y)} : sf::Vector2f{ static_cast<float>(mousePosOnMapX), static_cast<float>(mousePosOnMapY) };
				selectedCollisionLineIndex = cm->collLines.size();
				cm->collLines.push_back(new CollisionLine(lcpp.x, lcpp.y, mousePosOnMapX, mousePosOnMapY));
				cm->needPathfindingNodesRecalc = true;
				Sfx::Pick->play();
			}
		}
	}
	else newCollLineState = DrawNewCollLineState_Done;

	if (roadEditOn)
	{
		if (lastEditedRoad)
		{
			if (Mouse::LeftRel && !grabbedCollAnchor)
			{
				lastEditedRoad->anchors.push_back({ (int)mouseTilePosX, (int)mouseTilePosY });
				Sfx::Pick->play();
			}
			if (Mouse::RightRel && !lastEditedRoad->anchors.empty())
			{
				lastEditedRoad->anchors.pop_back();
				Sfx::Pick->play();
			}
		}
	}

	if (!Mouse::LeftFrames)
	{
		grabbedCollAnchor = false;
		grabbedRoadAnchorRoad = nullptr;
		grabbedRoadAnchorIdx = -1;
	}
#endif
}

void InGameScreen::showRadioStation(const std::string& stationName)
{
	radioDisplay.stationName = stationName;
	radioDisplay.alpha = 1.0f;
	radioDisplay.displayTime = 0.0f;
	radioDisplay.active = true;
}

void InGameScreen::showTemporaryMessage(const std::string& message, float duration, sf::Color color)
{
	tempMessage.message = message;
	tempMessage.displayTime = 0.0f;
	tempMessage.duration = duration;
	tempMessage.active = true;
	tempMessage.color = color;
}

void InGameScreen::InventoryPopup::draw(RendTarget* renderTarget, Actor* player)
{
	int inventoryWidth = GRID_WIDTH * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING;
	int inventoryHeight = GRID_HEIGHT * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING;
	int gridStartX = 120;
	int gridStartY = Game::ScreenHeight - inventoryHeight - 150;
	int totalSlots = GRID_WIDTH * GRID_HEIGHT;

	// Background
	sf::RectangleShape bg(sf::Vector2f(GRID_WIDTH * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING, GRID_HEIGHT * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING));
	bg.setPosition(gridStartX - SLOT_SPACING, gridStartY - SLOT_SPACING);
	bg.setFillColor(sf::Color(50, 55, 65, 200));
	bg.setOutlineColor(sf::Color::White);
	bg.setOutlineThickness(2);
	renderTarget->draw(bg);

	// Draw grid slots
	for(int i = 0; i < totalSlots; i++)
	{
		int x = i % GRID_WIDTH;
		int y = i / GRID_WIDTH;
		int slotX = gridStartX + x * (SLOT_SIZE + SLOT_SPACING);
		int slotY = gridStartY + y * (SLOT_SIZE + SLOT_SPACING);

		// Slot background
		sf::RectangleShape slot(sf::Vector2f(SLOT_SIZE, SLOT_SIZE));
		slot.setPosition(slotX, slotY);
		slot.setFillColor(sf::Color(60, 65, 75));
		slot.setOutlineColor(i == m_SelectedSlot ? sf::Color::Cyan : sf::Color(80, 85, 95));
		slot.setOutlineThickness(i == m_SelectedSlot ? 2 : 1);
		renderTarget->draw(slot);

		// Item in slot
		int itemIndex = m_ScrollOffset * GRID_WIDTH + i;
		if(itemIndex < (int)player->inv->items.size())
		{
			Item* item = player->inv->items[itemIndex];
			if(item->tex && item != m_DraggedItem)
			{
				float scale = 0.8f;
				item->sp.setPosition(slotX + SLOT_SIZE/2 - 8, slotY + SLOT_SIZE/2 - 8);
				item->sp.setScale(scale, scale);
				renderTarget->draw(item->sp);
			}

			// Equipped indicator
			if(item == player->inv->equippedItem && item != m_DraggedItem)
			{
				sf::CircleShape equippedDot(2);
				equippedDot.setPosition(slotX + SLOT_SIZE - 6, slotY + 2);
				equippedDot.setFillColor(sf::Color::Green);
				renderTarget->draw(equippedDot);
			}
		}
	}

	// Draw tooltip for hovered item
	int hoveredSlot = getSlotAtPosition(Mouse::Pos_X, Mouse::Pos_Y);
	if (hoveredSlot >= 0) {
		int itemIndex = m_ScrollOffset * GRID_WIDTH + hoveredSlot;
		if (itemIndex < (int)player->inv->items.size()) {
			Item* item = player->inv->items[itemIndex];
			sf::Text nameText(item->name, *Fonts::MainFont, 16);
			nameText.setFillColor(sf::Color::White);
			nameText.setOutlineColor(sf::Color::Black);
			nameText.setOutlineThickness(1);
			float tooltipX = Mouse::Pos_X + 10;
			float tooltipY = Mouse::Pos_Y - 30;
			nameText.setPosition(tooltipX, tooltipY);
			renderTarget->draw(nameText);

			sf::Text descText(item->description, *Fonts::MainFont, 12);
			descText.setFillColor(sf::Color::White);
			descText.setOutlineColor(sf::Color::Black);
			descText.setOutlineThickness(1);
			descText.setPosition(tooltipX, tooltipY + 20);
			renderTarget->draw(descText);
		}
	}

	// Draw dragged item
	if(m_DraggedItem && m_DraggedItem->tex)
	{
		m_DraggedItem->sp.setPosition(Mouse::Pos_X + m_DragOffsetX, Mouse::Pos_Y + m_DragOffsetY);
		m_DraggedItem->sp.setScale(0.8f, 0.8f);
		renderTarget->draw(m_DraggedItem->sp);
	}
}

void InGameScreen::InventoryPopup::handleInput(Actor* player)
{
	int inventoryWidth = GRID_WIDTH * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING;
	int inventoryHeight = GRID_HEIGHT * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING;
	int gridStartX = 120;
	int gridStartY = Game::ScreenHeight - inventoryHeight - 150;
	bool mouseInsideInventory = Mouse::Pos_X >= gridStartX && Mouse::Pos_Y >= gridStartY && Mouse::Pos_X <= gridStartX + GRID_WIDTH * (SLOT_SIZE + SLOT_SPACING) && Mouse::Pos_Y <= gridStartY + GRID_HEIGHT * (SLOT_SIZE + SLOT_SPACING);

	// Controller navigation using d-pad
	bool dpadLeft = (Controller::dpx == -100 && Controller::dpxFrames == 1);
	bool dpadRight = (Controller::dpx == 100 && Controller::dpxFrames == 1);
	bool dpadUp = (Controller::dpy == 100 && Controller::dpyFrames == 1);
	bool dpadDown = (Controller::dpy == -100 && Controller::dpyFrames == 1);

	if(player->inv->items.empty()) return;

	int oldSlot = m_SelectedSlot;
	int oldScroll = m_ScrollOffset;

	// Grid navigation with d-pad
	if(dpadLeft)
	{
		m_SelectedSlot--;
		if(m_SelectedSlot < 0) m_SelectedSlot = GRID_WIDTH - 1;
	}
	if(dpadRight)
	{
		m_SelectedSlot++;
		if(m_SelectedSlot >= GRID_WIDTH) m_SelectedSlot = 0;
	}
	if(dpadUp)
	{
		m_SelectedSlot -= GRID_WIDTH;
		if(m_SelectedSlot < 0)
		{
			if(m_ScrollOffset > 0)
			{
				m_ScrollOffset--;
				m_SelectedSlot += GRID_WIDTH;
			}
			else
			{
				m_SelectedSlot += GRID_WIDTH;
			}
		}
	}
	if(dpadDown)
	{
		m_SelectedSlot += GRID_WIDTH;
		if(m_SelectedSlot >= GRID_WIDTH * GRID_HEIGHT)
		{
			int totalRows = (player->inv->items.size() + GRID_WIDTH - 1) / GRID_WIDTH;
			if(m_ScrollOffset < totalRows - GRID_HEIGHT)
			{
				m_ScrollOffset++;
				m_SelectedSlot -= GRID_WIDTH;
			}
			else
			{
				m_SelectedSlot -= GRID_WIDTH;
			}
		}
	}

	// Clamp selection to valid items
	int itemIndex = getSelectedItemIndex();
	if(itemIndex >= (int)player->inv->items.size())
	{
		m_SelectedSlot = oldSlot;
		m_ScrollOffset = oldScroll;
	}

	// Equip item with Square button
	if(Controller::BtnFrames[Btn_Square] == 1)
	{
		int idx = getSelectedItemIndex();
		if(idx < (int)player->inv->items.size())
		{
			player->inv->equippedItem = player->inv->items[idx];
		}
		else
		{
			player->inv->equippedItem = nullptr;
		}
	}

	// Drop item with Triangle button
	if(Controller::BtnFrames[Btn_Triangle] == 1)
	{
		int idx = getSelectedItemIndex();
		if(idx < (int)player->inv->items.size())
		{
			Item* itemToDrop = player->inv->items[idx];

			// Unequip if equipped
			if(itemToDrop == player->inv->equippedItem)
			{
				player->inv->equippedItem = nullptr;
			}

			// Remove from inventory
			player->inv->items.erase(player->inv->items.begin() + idx);

			// Calculate position in front of player
			double dropPosX = player->posX;
			double dropPosY = player->posY;
			if(player->getDir() == Direction_Up) dropPosY -= 20;
			else if(player->getDir() == Direction_Down) dropPosY += 20;
			else if(player->getDir() == Direction_Left) dropPosX -= 20;
			else if(player->getDir() == Direction_Right) dropPosX += 20;

			// Set item position and add to map
			itemToDrop->posX = dropPosX;
			itemToDrop->posY = dropPosY;
			itemToDrop->updateTPos();
			itemToDrop->inv = nullptr;

			player->hostMap->addEnt(itemToDrop);
		}
	}

	// Close popup with Circle button
	if(Controller::BtnFrames[Btn_Circle] == 1)
	{
		IGS->showInventoryPopup = false;
		return;
	}

	// Left click to select or start drag
	if(Mouse::LeftFrames == 1 && !m_DraggedItem)
	{
		int slot = getSlotAtPosition(Mouse::Pos_X, Mouse::Pos_Y);
		if(slot >= 0)
		{
			int itemIndex = m_ScrollOffset * GRID_WIDTH + slot;
			m_SelectedSlot = slot;
			if(itemIndex < (int)player->inv->items.size())
			{
				player->inv->equippedItem = player->inv->items[itemIndex];
				m_DraggedItem = player->inv->items[itemIndex];
				m_DraggedFromIndex = itemIndex;
				int x = slot % GRID_WIDTH;
				int y = slot / GRID_WIDTH;
				int slotX = gridStartX + x * (SLOT_SIZE + SLOT_SPACING);
				int slotY = gridStartY + y * (SLOT_SIZE + SLOT_SPACING);
				m_DragOffsetX = slotX - Mouse::Pos_X;
				m_DragOffsetY = slotY - Mouse::Pos_Y;
			}
			else
			{
				// Empty slot: de-equip
				player->inv->equippedItem = nullptr;
			}
		}
	}
	
	// End drag
	if(Mouse::LeftRel && m_DraggedItem)
	{
		int slot = getSlotAtPosition(Mouse::Pos_X, Mouse::Pos_Y);
		if(slot >= 0)
		{
			int targetIndex = m_ScrollOffset * GRID_WIDTH + slot;
			if(targetIndex != m_DraggedFromIndex)
			{
				// Remove from original position
				player->inv->items.erase(player->inv->items.begin() + m_DraggedFromIndex);
				
				// Insert at new position
				if(targetIndex >= (int)player->inv->items.size())
				{
					player->inv->items.push_back(m_DraggedItem);
				}
				else
				{
					if(targetIndex > m_DraggedFromIndex) targetIndex--;
					player->inv->items.insert(player->inv->items.begin() + targetIndex, m_DraggedItem);
				}
			}
		}
		else
		{
			// Drop in world
			Item* itemToDrop = m_DraggedItem;

			// Remove from inventory
			if(itemToDrop == player->inv->equippedItem)
			{
				player->inv->equippedItem = nullptr;
			}
			player->inv->items.erase(player->inv->items.begin() + m_DraggedFromIndex);

			// Calculate position in front of player
			double dropPosX = player->posX;
			double dropPosY = player->posY;
			if(player->getDir() == Direction_Up) dropPosY -= 20;
			else if(player->getDir() == Direction_Down) dropPosY += 20;
			else if(player->getDir() == Direction_Left) dropPosX -= 20;
			else if(player->getDir() == Direction_Right) dropPosX += 20;

			// Set item position and add to map
			itemToDrop->posX = dropPosX;
			itemToDrop->posY = dropPosY;
			itemToDrop->updateTPos();
			itemToDrop->inv = nullptr;

			player->hostMap->addEnt(itemToDrop);
		}
		m_DraggedItem = nullptr;
		m_DraggedFromIndex = -1;
	}
	else if(Mouse::LeftRel && !mouseInsideInventory)
	{
		IGS->showInventoryPopup = false;
	}
}

int InGameScreen::InventoryPopup::getSlotAtPosition(int posX, int posY) const
{
	int inventoryWidth = GRID_WIDTH * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING;
	int inventoryHeight = GRID_HEIGHT * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING;
	int gridStartX = 120;
	int gridStartY = Game::ScreenHeight - inventoryHeight - 150;
	
	if(posX < gridStartX || posY < gridStartY) return -1;
	
	int x = (posX - gridStartX) / (SLOT_SIZE + SLOT_SPACING);
	int y = (posY - gridStartY) / (SLOT_SIZE + SLOT_SPACING);
	
	if(x >= GRID_WIDTH || y >= GRID_HEIGHT) return -1;
	if((posX - gridStartX) % (SLOT_SIZE + SLOT_SPACING) >= SLOT_SIZE) return -1;
	if((posY - gridStartY) % (SLOT_SIZE + SLOT_SPACING) >= SLOT_SIZE) return -1;
	
	return y * GRID_WIDTH + x;
}

int InGameScreen::InventoryPopup::getSelectedItemIndex() const
{
	return m_ScrollOffset * GRID_WIDTH + m_SelectedSlot;
}

void InGameScreen::otherScreenUpdate()
{
	auto cm = mapFactory->getCurrentMap();
	gameClock->tick();
 	cm->update();
	curMission->update();
}

} // namespace nyaa
