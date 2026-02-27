#include "Entity/CarBase.h"
#include "Map/Pathfinding.h"
#include "Map/CollisionLine.h"
#include "Entity/Interaction.h"
#include <cassert>
#include <cmath>
#include "Screen/InGameScreen.h"
#include "Screen/PauseScreen.h"
#include "Entity/Actor.h"
#include "Game/Util.h"
#include <SFML/Window/Keyboard.hpp>
#include "Game/Game.h"
#include "Map/Map.h"
#include "Texture/Sheet.h"
#include <SFML/Window/Joystick.hpp>
#include "Game/Controller.h"
#include "Game/Sfx.h"
#include "Game/EventScheduler.h"
#include "Game/FMRadio.h"
#include "Inventory/Inventory.h"
#include "Game/OBB.h"
#include "Inventory/GasCan.h"
#include "Entity/BuildingWallEntity.h"

namespace nyaa {

CarBase::CarBase(float mass_, float inertia_, float sizeX_, float sizeY_,
                 float originX_, float originY_, float friction_, float wheelBase_,
                 const std::vector<Wheel>& wheels_, float driftDuration_, float driftSteeringMultiplier_,
                 float hoodHeight_, float roofHeight_)
    :velX(0.f)
    ,velY(0.f)
    ,wheelBase(wheelBase_) // meters (approx, visual scaling might be needed)
    ,moveSpeed(0.0f)
    ,occupant(nullptr)
    ,collisionSoundPlayed(false)
    ,health(100.0f)
    ,bodyRoll(0.0f)
    ,hoodHeight(hoodHeight_)
    ,roofHeight(roofHeight_)
    ,fuelLevel((std::rand() % 61) + 20.0f) // Start with random fuel between 20-80%
    ,isOutOfFuel(false)
    ,isDrifting(false)
    ,driftTimer(0.0f)
    ,driftDuration(driftDuration_) // 1.5 seconds max drift
    ,driftSteeringMultiplier(driftSteeringMultiplier_) // More responsive steering during drift
    ,isAutoDriving(false)
    ,autoDriveTargetX(0.0f)
    ,autoDriveTargetY(0.0f)
    ,ast(nullptr)
    ,astn(nullptr)
    ,reSolveAST(false)
    ,directPath(false)
    ,lastTargetX(0.0)
    ,lastTargetY(0.0)
    ,autoDriveStuckTimer(0.0f)
    ,autoDriveReverseTimer(0.0f)
    ,isAutoDriveReversing(false)
    ,lastFrameCollided(false)
	,gearSwitchTimer(0.0f)
	,isGearReversing(false)
    ,getInInter(nullptr)
    ,getOutInter(nullptr)
    ,prevRadioStationInter(nullptr)
    ,nextRadioStationInter(nullptr)
    ,fillTankInter(nullptr)
,steerInput(0.0f)
	,throttleInput(0.0f)
	,brakeInput(0.0f)
	,cameraRefAngle(0.0f)
	,cameraControlActive(false)
	,lastStickAngle(0.0f)
{
    // Initialize physics properties
    mass = mass_; // kg
    inertia = inertia_; // (1/12) * M * (L^2 + W^2) for 64x32 box
    sizeX = sizeX_;
    sizeY = sizeY_;
    originX = originX_;
    originY = originY_;
    friction = friction_;

    // Initialize wheels
    wheels = wheels_;

	getInInter = new Interaction(this, "Get in", [](Interaction* i) -> bool
	{
		CarBase* car = (CarBase*) i->e;
		assert(i->user == IGS->player);
		car->occupant = i->user;
		car->occupant->carImInsideOf = car;
		i->user->hostMap->removeEnt(i->user);
		Sfx::CarDoorOpen->play((float)car->posX, (float)car->posY);
		
		// Start radio when getting in the car, but only if the game is not paused
		if (IGS->fmRadio && Screen::CurrentScreen != PauseScreen::Instance) {
			IGS->fmRadio->PickRandomSong();
			IGS->fmRadio->Resume(); // Ensure radio is playing
			// Show current radio station when getting in the car
			IGS->showRadioStation(RadioStation::GetStationName(IGS->fmRadio->getCurrentStation()));
		}
		
		float cpx = (float)car->posX, cpy = (float)car->posY;
		EventScheduler::GlobalScheduler->scheduleEvent(320, [cpx, cpy]() {
			Sfx::CarDoorClose->play(cpx, cpy);
		});

		car->isGearReversing = false;
		car->gearSwitchTimer = 0.0f;

		return true;
	});

	getOutInter = new Interaction(this, "Get out", [](Interaction* i) -> bool
	{
		CarBase* car = (CarBase*) i->e;
		if (car->occupant)
		{
			car->occupant->hostMap->addEnt(car->occupant);
			//car->occupant->setPosTile(car->tilePos.x, car->tilePos.y + 1);
			Point np = {};
			Util::AngleLineRel(car->posX, car->posY, (double)std::fmod(car->dirAngle - 90.0f, 360.0f), 20.0, &np.x, &np.y);
			car->occupant->setPos(np.x, np.y);
			car->occupant->carImInsideOf = nullptr;
			car->occupant = nullptr;
			
			Sfx::CarDoorOpen->play((float)car->posX, (float)car->posY);
			IGS->fmRadio->Stop();
			float cpx = (float)car->posX, cpy = (float)car->posY;
			EventScheduler::GlobalScheduler->scheduleEvent(600, [cpx, cpy]() {
				Sfx::CarDoorClose->play((float)cpx, (float)cpy);
			});
		}
		return true;
	});

	prevRadioStationInter = new Interaction(this, "Previous radio station", [](Interaction* i) -> bool
	{
		IGS->fmRadio->PrevStation();
		return true;
	});

	nextRadioStationInter = new Interaction(this, "Next radio station", [](Interaction* i) -> bool
	{
		IGS->fmRadio->NextStation();
		return true;
	});

	fillTankInter = new Interaction(this, "Fill tank", [](Interaction* i) -> bool
	{
		CarBase* car = (CarBase*) i->e;
		// Use a filled gas can from player's inventory
		if (IGS->player->inv->useFilledGasCan()) {
			// Fill the car's tank
			car->fuelLevel = 100.0f;
			car->isOutOfFuel = false;
			return true;
		}
		return false;
	});
}

CarBase::~CarBase()
{
    if (ast) delete ast;
}

void CarBase::forward(float spd)
{
	throttleInput += spd;
}

void CarBase::backward(float spd)
{
	throttleInput -= spd;
}

void CarBase::left(float spd)
{
	//float speedFactor = std::max(0.1f, std::abs(moveSpeed) / 100.0f);
	//float effectiveSteer = spd * (0.5f + (speedFactor * 0.5f));
	//float maxSteerRate = 2.0f / (1.0f + (speedFactor * 2.0f));
	//if (effectiveSteer > maxSteerRate) effectiveSteer = maxSteerRate;
	//if (effectiveSteer < -maxSteerRate) effectiveSteer = -maxSteerRate;
	steerInput -= /*effectiveSteer*/ spd;
}

void CarBase::right(float spd)
{
	//float speedFactor = std::max(0.1f, std::abs(moveSpeed) / 100.0f);
	//float effectiveSteer = spd * (0.5f + (speedFactor * 0.5f));
	//float maxSteerRate = 2.0f / (1.0f + (speedFactor * 2.0f));
	//if (effectiveSteer > maxSteerRate) effectiveSteer = maxSteerRate;
	//if (effectiveSteer < -maxSteerRate) effectiveSteer = -maxSteerRate;
	steerInput += /*effectiveSteer*/ spd;
}

void CarBase::update()
{
	if (occupant == IGS->player)
	{
#if DEBUG
		//teleport
		if (Kb::IsKeyDown(KB::LShift) && Kb::IsKeyFirstFrame(KB::P))
		{
			posX = IGS->mousePosOnMapX;
			posY = IGS->mousePosOnMapY;
		}
#endif
		IGS->fmRadio->Tick();
	}

	Entity::update();
	if (!occupant && Util::Dist(IGS->player->posX, IGS->player->posY, posX, posY) < 25)
	{
		IGS->addInteractablePriority(getInInter);

		// Check if player has a filled gas can
		if (IGS->player->inv->hasFilledGasCan()) {
			IGS->addInteractablePriority(fillTankInter);
		}
	}
	
	float dt = G->frameDeltaMillis / 1000.0f;
	if (dt > 0.1f) dt = 0.1f; // Cap dt to avoid explosion

	steerInput = 0.0f;
	throttleInput = 0.0f;
	brakeInput = 0.0f;

    // Calculate speed for drift logic and auto-drive
	float speed = std::sqrt(velX * velX + velY * velY);

	//controls
	//controls
	if (isAutoDriving)
	{
		// Pathfinding Management
		if (astClock.getElapsedMilliseconds() >= 1000 || reSolveAST)
		{
			astClock.restart();
			if (reSolveAST || Util::Dist(lastTargetX, lastTargetY, autoDriveTargetX, autoDriveTargetY) > 50.0f)
			{
				lastTargetX = autoDriveTargetX;
				lastTargetY = autoDriveTargetY;
				if (ast) delete ast;
				ast = new Pathfinding();
				ast->loadNodes(hostMap, posX, posY, autoDriveTargetX, autoDriveTargetY);
				ast->solve();
				astn = ast->startNode;
				reSolveAST = false;
				
				// Handle failed pathfinding - fallback to direct if close or nothing better
				if (!astn) directPath = true;
				else directPath = false;
			}
		}

		// Determine current intermediate target
		float currentTargX = autoDriveTargetX;
		float currentTargY = autoDriveTargetY;

		if (astn && !directPath) {
			currentTargX = (float)astn->posX;
			currentTargY = (float)astn->posY;
			
			// If we reached the next node, move to the one after it
			if (Util::Dist(posX, posY, currentTargX, currentTargY) < 60.0f) {
				if (astn->child) {
					astn = astn->child;
					currentTargX = (float)astn->posX;
					currentTargY = (float)astn->posY;
				} else {
					// End of path
					directPath = true;
				}
			}
		}

		float distToTarget = Util::Dist(posX, posY, currentTargX, currentTargY);
		float finalDist = Util::Dist(posX, posY, autoDriveTargetX, autoDriveTargetY);
		
		if (finalDist < 50.0f && speed < 20.0f) {
			stopAutoDrive();
		}
		else {
			// Calculate direction to target
			float angleToTarget = Util::RotateTowards(posX, posY, currentTargX, currentTargY);
			
			// Normalize car angle to 0-360
			float currentAngle = std::fmod(dirAngle, 360.0f);
			if (currentAngle < 0) currentAngle += 360.0f;
            
            // Normalize target angle
             if (angleToTarget < 0) angleToTarget += 360.0f;
			
			float angleDiff = angleToTarget - currentAngle;
			if (angleDiff > 180) angleDiff -= 360;
			if (angleDiff < -180) angleDiff += 360;
			
            // Basic steering towards target
			if (angleDiff > 10.0f) right(1.0f);
			else if (angleDiff < -10.0f) left(1.0f);
			else right(angleDiff / 10.0f);
			
			// Obstacle Avoidance (Whiskers)
            // Look ahead distance increases with speed
            float lookAheadDist = 180.0f + speed * 1.8f;
            
            // Whiskers: Center, Left, Right, and slight diagonals
            std::vector<float> whiskerAngles = { 0.0f, -25.0f, 25.0f, -12.0f, 12.0f };
            float avoidanceSteer = 0.0f;
            bool obstacleDetected = false;
            
            for (float wa : whiskerAngles) {
                float rad = Util::ToRad(dirAngle + wa);
                float rayEndX = posX + std::cos(rad) * lookAheadDist;
                float rayEndY = posY + std::sin(rad) * lookAheadDist;
                
                bool hit = false;
                // Check map collision lines
                for (auto cl : hostMap->collLines) {
                    if (Util::Intersects(posX, posY, rayEndX, rayEndY, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y)) {
                        hit = true;
                        break;
                    }
                }
                
                // Check entities
                if (!hit) {
                    for (auto e : hostMap->getEntities()) {
                        if (e == this || (e->flags & EntFlag_UnderAllOtherEnts) || (e->flags & EntFlag_Dead)) continue;
                        OBB* eOBB = e->getOBB();
                        if (OBBIntersectsLine(*eOBB, posX, posY, rayEndX, rayEndY, nullptr)) {
                            hit = true;
                            break;
                        }
                    }
                }

                if (hit) {
                    obstacleDetected = true;
                    // Steer AWAY from the obstacle
                    if (wa < 0) avoidanceSteer += 1.0f; // Obstacle on left, steer right
                    else if (wa > 0) avoidanceSteer -= 1.0f; // Obstacle on right, steer left
                    else {
                        // Obstacle straight ahead, steer towards open space or default right
                        avoidanceSteer += (steerInput > 0 ? 1.0f : -1.0f);
                    }
                }
            }
            
            if (obstacleDetected) {
                steerInput = Util::Clamp(steerInput + avoidanceSteer * 2.0f, -1.0f, 1.0f);
                // Slow down if obstacle detected
                throttleInput = 0.4f; 
                 if (speed > 80.0f) brakeInput = 0.8f;
            } else {
                 // Cornering speed control
                if (std::abs(angleDiff) > 40.0f && speed > 70.0f) {
                     throttleInput = 0.0f;
                     brakeInput = 0.6f;
                } else if (std::abs(angleDiff) > 15.0f && speed > 130.0f) {
                     throttleInput = 0.0f; // Coast
                } else {
                    throttleInput = 1.0f;
                }
            }
            
            // Reverse if stuck
            if (isAutoDriveReversing) {
                autoDriveReverseTimer -= dt;

                // Premature stop if we have LOS to target and front is clear
                // Only check after a minimum reversal duration (0.4s) to prevent oscillations
                if (autoDriveReverseTimer < 2.4f) {
                    float losDist = (float)Util::Dist(posX, posY, autoDriveTargetX, autoDriveTargetY);
                    OBB losOBB;
                    losOBB.center = { (posX + autoDriveTargetX) * 0.5, (posY + autoDriveTargetY) * 0.5 };
                    losOBB.angle = (float)Util::RotateTowards(posX, posY, (float)autoDriveTargetX, (float)autoDriveTargetY);
                    losOBB.halfExtents = { losDist * 0.5f, 20.0f }; // Width margin for the car

                    bool losBlocked = false;
                    for (auto cl : hostMap->collLines) {
                        if (OBBIntersectsLine(losOBB, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y, nullptr)) {
                            losBlocked = true;
                            break;
                        }
                    }
                    
                    if (!losBlocked) {
                        // Check if immediate front is clear enough to drive forward
                        bool pathClear = true;
                        float checkD = 120.0f; 
                        for (float wa : {0.0f, -20.0f, 20.0f}) {
                            float rad = Util::ToRad(dirAngle + wa);
                            float rX = posX + std::cos(rad) * checkD;
                            float rY = posY + std::sin(rad) * checkD;
                            for (auto cl : hostMap->collLines) {
                                if (Util::Intersects(posX, posY, rX, rY, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y)) {
                                    pathClear = false; break;
                                }
                            }
                            if (!pathClear) break;
                            for (auto e : hostMap->getEntities()) {
                                if (e == this || (e->flags & EntFlag_UnderAllOtherEnts) || (e->flags & EntFlag_Dead)) continue;
                                OBB* eOBB = e->getOBB();
                                if (OBBIntersectsLine(*eOBB, posX, posY, rX, rY, nullptr)) {
                                    pathClear = false; break;
                                }
                            }
                            if (!pathClear) break;
                        }

                        if (pathClear) {
                            isAutoDriveReversing = false;
                            autoDriveStuckTimer = 0.0f;
                            reSolveAST = true;
                        }
                    }
                }
                
                // Backward Whiskers for avoidance while reversing
                float backwardDist = 150.0f;
                std::vector<float> bWhiskers = { 180.0f, 155.0f, 205.0f };
                float bAvoidance = 0.0f;
                bool bObstacle = false;
                
                for (float wa : bWhiskers) {
                    float rad = Util::ToRad(dirAngle + wa);
                    float rEndX = posX + std::cos(rad) * backwardDist;
                    float rEndY = posY + std::sin(rad) * backwardDist;
                    
                    bool hit = false;
                    for (auto cl : hostMap->collLines) {
                        if (Util::Intersects(posX, posY, rEndX, rEndY, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y)) {
                            hit = true;
                            break;
                        }
                    }
                    
                    if (!hit) {
                        for (auto e : hostMap->getEntities()) {
                            if (e == this || (e->flags & EntFlag_UnderAllOtherEnts) || (e->flags & EntFlag_Dead)) continue;
                            OBB* eOBB = e->getOBB();
                            if (OBBIntersectsLine(*eOBB, posX, posY, rEndX, rEndY, nullptr)) {
                                hit = true;
                                break;
                            }
                        }
                    }

                    if (hit) {
                        bObstacle = true;
                        if (wa < 180.0f) bAvoidance += 1.0f;
                        else if (wa > 180.0f) bAvoidance -= 1.0f;
                        else bAvoidance += (steerInput > 0 ? 1.0f : -1.0f);
                    }
                }
                
                throttleInput = -0.9f; // Reverse with more power
                if (bObstacle) {
                    steerInput = Util::Clamp(bAvoidance * 1.5f, -1.0f, 1.0f);
                } else {
                    // Steer in a way that swings the nose TOWARDS the target
                    if (angleDiff > 0) steerInput = -1.0f; // More aggressive steering
                    else steerInput = 1.0f;
                }
                
                if (autoDriveReverseTimer <= 0.0f) {
                    bool frontClear = true;
                    float checkDist = 220.0f;
                    for (float wa : {0.0f, -30.0f, 30.0f}) {
                        float rad = Util::ToRad(dirAngle + wa);
                        float rX = posX + std::cos(rad) * checkDist;
                        float rY = posY + std::sin(rad) * checkDist;
                        
                        bool hit = false;
                        for (auto cl : hostMap->collLines) {
                            if (Util::Intersects(posX, posY, rX, rY, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y)) {
                                hit = true;
                                break;
                            }
                        }
                        
                        if (!hit) {
                            for (auto e : hostMap->getEntities()) {
                                if (e == this || (e->flags & EntFlag_UnderAllOtherEnts) || (e->flags & EntFlag_Dead)) continue;
                                OBB* eOBB = e->getOBB();
                                if (OBBIntersectsLine(*eOBB, posX, posY, rX, rY, nullptr)) {
                                    hit = true;
                                    break;
                                }
                            }
                        }

                        if (hit) {
                            frontClear = false;
                            break;
                        }
                    }

                    if (frontClear) {
                        isAutoDriveReversing = false;
                        autoDriveStuckTimer = 0.0f;
                        reSolveAST = true;
                    } else {
                        autoDriveReverseTimer = 0.6f; 
                        if (autoDriveStuckTimer > 15.0f) { 
                            isAutoDriveReversing = false;
                            autoDriveStuckTimer = 0.0f;
                            reSolveAST = true;
                        }
                    }
                }
            } else {
                // Improved stuck detection
                bool wallOrEntVeryClose = false;
                float shortDist = 75.0f; // Just in front of the car
                for (float wa : {0.0f, -20.0f, 20.0f}) {
                    float rad = Util::ToRad(dirAngle + wa);
                    float rX = posX + std::cos(rad) * shortDist;
                    float rY = posY + std::sin(rad) * shortDist;
                    
                    bool hit = false;
                    for (auto cl : hostMap->collLines) {
                        if (Util::Intersects(posX, posY, rX, rY, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y)) {
                            hit = true;
                            break;
                        }
                    }
                    
                    if (!hit) {
                        for (auto e : hostMap->getEntities()) {
                            if (e == this || (e->flags & EntFlag_UnderAllOtherEnts) || (e->flags & EntFlag_Dead)) continue;
                            OBB* eOBB = e->getOBB();
                            if (OBBIntersectsLine(*eOBB, posX, posY, rX, rY, nullptr)) {
                                hit = true;
                                break;
                            }
                        }
                    }

                    if (hit) {
                        wallOrEntVeryClose = true;
                        break;
                    }
                }

                bool stuck = (speed < 20.0f && std::abs(throttleInput) > 0.1f);
                if (lastFrameCollided && speed < 15.0f) stuck = true;
                if (wallOrEntVeryClose && speed < 18.0f) stuck = true;
                
                if (stuck) {
                    autoDriveStuckTimer += dt;
                    // Trigger even faster if wall is very close
                    float triggerTime = wallOrEntVeryClose ? 0.35f : 0.75f;
                    if (autoDriveStuckTimer > triggerTime) {
                        isAutoDriveReversing = true;
                        autoDriveReverseTimer = 2.8f; // Initial reverse time
                    }
                } else {
                    autoDriveStuckTimer = std::max(0.0f, autoDriveStuckTimer - dt);
                }
            }
		}
	}
	else if (occupant)
	{
		if (occupant == IGS->player)
		{
            IGS->player->updateTrips();
			IGS->interactables.push_back(getOutInter);
			IGS->interactables.push_back(prevRadioStationInter);
			IGS->interactables.push_back(nextRadioStationInter);
			if(G->inMethod == InputMethod_Keyboard)
			{
				if (Kb::IsKeyDown(KB::W)) forward(1.0f);
				if (Kb::IsKeyDown(KB::S)) backward(1.0f);
				if (Kb::IsKeyDown(KB::A)) left(1.0f);
				if (Kb::IsKeyDown(KB::D)) right(1.0f);
			}
else if(G->inMethod == InputMethod_Controller)
				{
					auto& ji = G->joystickIndex;
					float joyX = sf::Joystick::getAxisPosition(ji, sf::Joystick::Axis::X);
					float joyY = sf::Joystick::getAxisPosition(ji, sf::Joystick::Axis::Y);
					auto deadzone = 15.f;
					if (std::fabs(joyX) < deadzone) joyX = 0;
					if (std::fabs(joyY) < deadzone) joyY = 0;

					if (joyX != 0 || joyY != 0) {
						// Calculate stick angle (in degrees, 0 = right, 90 = down, 180 = left, 270 = up)
						float stickAngle = std::atan2(joyY, joyX) * (180.0f / 3.14159265f);
						
						// Normalize stick angle to 0-360
						if (stickAngle < 0) stickAngle += 360.0f;
						
						// Calculate stick magnitude (how far the stick is pushed)
						float stickMagnitude = std::sqrt(joyX * joyX + joyY * joyY) / 100.0f;
						if (stickMagnitude > 1.0f) stickMagnitude = 1.0f;
						
						// Camera-relative controls:
						// Stick direction is relative to the camera view
						// We want: stick pointing "down" on screen = drive forward regardless of car rotation
						//          stick pointing "left" on screen = steer left relative to camera
						
						// Get car's current facing angle (normalized to 0-360)
						float carAngle = std::fmod(dirAngle, 360.0f);
						if (carAngle < 0) carAngle += 360.0f;
						
						// In the game coordinate system:
						// dirAngle 0 = facing right (east)
						// dirAngle 90 = facing down (south)
						// dirAngle 180 = facing left (west)
						// dirAngle 270 = facing up (north)
						
						// The stick angle is also in this coordinate system
						// stickAngle 0 = stick pushed right
						// stickAngle 90 = stick pushed down
						// stickAngle 180 = stick pushed left
						// stickAngle 270 = stick pushed up
						
						// Calculate the relative angle between stick and car facing
						float relativeAngle = stickAngle - carAngle;
						// Normalize to -180 to 180
						while (relativeAngle > 180.0f) relativeAngle -= 360.0f;
						while (relativeAngle < -180.0f) relativeAngle += 360.0f;
						
// Now interpret the relative angle with 66% forward / 34% reverse bias:
					// Forward arc: -120° to +120° (240° total = 66.7%)
					// Reverse arc: 120° to 240° (120° total = 33.3%)
					// This makes forward driving more likely and reduces accidental gear shifts
					const float FORWARD_HALF_ANGLE = 120.0f; // Half of the forward arc
					
					// Check if we're in forward or reverse zone based on the biased angle ranges
					bool inForwardZone = std::fabs(relativeAngle) < FORWARD_HALF_ANGLE;
					bool inReverseZone = !inForwardZone;
					
					// Calculate steering - we need to scale the angle so that:
					// In forward zone: -120° to +120° maps to steering -1 to +1
					// In reverse zone: the remaining 60° on each side maps to steering -1 to +1
					float steeringAngle;
					if (inForwardZone) {
						// Scale the forward zone: -120° to +120° should feel like full steering range
						// We multiply by (90/120) = 0.75 to normalize to -90° to +90° equivalent
						steeringAngle = relativeAngle * 0.75f;
					} else {
						// In reverse zone: remaining angles (120° to 180° and -180° to -120°)
						// Scale these to feel like the outer steering range
						// Map 120°-180° to 90°-180° equivalent (and similarly for negative)
						if (relativeAngle > 0) {
							// 120° to 180°
							steeringAngle = 90.0f + (relativeAngle - FORWARD_HALF_ANGLE) * 1.5f;
						} else {
							// -180° to -120°
							steeringAngle = -90.0f + (relativeAngle + FORWARD_HALF_ANGLE) * 1.5f;
						}
					}
					
					// Use sine for steering based on the scaled angle
					steerInput = std::sin(Util::ToRad(steeringAngle));
					
					// Calculate throttle: full magnitude in the respective zone
					if (inForwardZone) {
						throttleInput = stickMagnitude;
					} else {
						throttleInput = -stickMagnitude;
					}
						
						// Store for reference
						lastStickAngle = stickAngle;
						cameraControlActive = true;
					} else {
						cameraControlActive = false;
					}

					if (Controller::BtnFrames[Btn_Circle]) brakeInput = 1.0f;
				}
			
            if (Controller::BtnFrames[Btn_X] || Kb::IsKeyDown(KB::Space)) brakeInput = 1.0f;
			
			occupant->posX = posX;
			occupant->posY = posY;
		}
	}

	// Gear switching logic
	if (throttleInput > 0.01f && isGearReversing)
	{
		isGearReversing = false;
		gearSwitchTimer = 0.7f;
		Sfx::CarGearShift->play((float)posX, (float)posY, true);
	}
	else if (throttleInput < -0.01f && !isGearReversing)
	{
		isGearReversing = true;
		gearSwitchTimer = 0.7f;
		Sfx::CarGearShift->play((float)posX, (float)posY, true);
	}

	if (gearSwitchTimer > 0.0f)
	{
		gearSwitchTimer -= dt;
		throttleInput = 0.0f;
	}

    // Fuel consumption (after inputs are set)
    if (fuelLevel > 0.0f && (occupant != nullptr || isAutoDriving)) {
        // Baseline idle consumption + throttle-based consumption
        float consumptionRate = 0.02f + (std::abs(throttleInput) * 0.48f);
        fuelLevel -= consumptionRate * dt;
        if (fuelLevel < 0.0f) fuelLevel = 0.0f;
    }

    // Check if out of fuel
    isOutOfFuel = (fuelLevel <= 0.0f);

// --- Physics Simulation ---

	// Constants
	const float MAX_STEER_ANGLE = Util::ToRad(40.0f);
	const float ENGINE_POWER = 150000.0f;
	const float MAX_BRAKE_FORCE = 600000.0f;
	const float DRAG_COEFF = 6.0f;   // Increased from 2.0f to 6.0f for more air resistance
	const float ROLLING_RESISTANCE = 2000.0f; // Increased from 500.0f to 2000.0f for more friction
	const float CORNERING_STIFFNESS = 600000.0f;
	
	// Drift mechanics
	bool wantsToDrift = (Kb::IsKeyDown(KB::LShift) || Controller::BtnFrames[Btn_Circle]);
	
	// Speed is already calculated above
	
	// Handle drift state
	if (wantsToDrift && !isDrifting && speed > 50.0f) {
		// Start drift
		isDrifting = true;
		driftTimer = driftDuration;
		// Play drift sound
		Sfx::CarDrift->play((float)posX, (float)posY, true);
	} else if (isDrifting) {
		// Continue or end drift
		driftTimer -= dt;
		if (driftTimer <= 0.0f || !wantsToDrift || speed < 20.0f) {
			isDrifting = false;
			// Stop drift sound
			Sfx::CarDrift->stop();
		}
	}
	// Apply drift effects
	float enginePowerMultiplier = 1.0f;
	float rearWheelGripMultiplier = 1.0f;
	float steeringMultiplier = 1.0f;
	
	if (isDrifting) {
		enginePowerMultiplier = 1.5f; // More power during drift
		rearWheelGripMultiplier = 0.1f; // Reduce rear wheel grip for sliding
		steeringMultiplier = driftSteeringMultiplier; // More responsive steering
	}

	float forceSumX = 0.f;
	float forceSumY = 0.f;
	float torqueSum = 0.f;

	float angleRad = Util::ToRad(dirAngle);
	
	// Update wheel steering
	// Scale steering speed based on current speed for more realistic handling
	float speedFactor = std::max(0.1f, std::abs(speed) / 100.0f);
	float steerSpeed = (8.0f - (speedFactor * 4.0f)) * dt; // Slower steering at high speed
	
	// Apply drift steering multiplier
	if (isDrifting) {
		steerInput *= steeringMultiplier;
	}
	
	for (auto& w : wheels) {
		if (w.steerable) {
			float targetAngle = steerInput * MAX_STEER_ANGLE;
	          if (steerInput == 0.0f) {
	              // Snap back faster when no input, but slower at high speed
	              float snapBackFactor = 1.0f + (speedFactor * 10.0f);
	              w.steerAngle /= (1.0f + snapBackFactor * dt);
	              if (std::abs(w.steerAngle) < 0.001f) w.steerAngle = 0;
	          } else {
	              float diff = targetAngle - w.steerAngle;
	              w.steerAngle += diff * steerSpeed;
	          }
		}
	}
	
	for (auto& w : wheels) {
		// Wheel Current Orientation
		float wx = w.localX * std::cos(angleRad) - w.localY * std::sin(angleRad);
		float wy = w.localX * std::sin(angleRad) + w.localY * std::cos(angleRad);

		float wheelAngleRad = angleRad + w.steerAngle;
		float wheelDirX = std::cos(wheelAngleRad);
		float wheelDirY = std::sin(wheelAngleRad);
		float wheelSideX = -std::sin(wheelAngleRad);
		float wheelSideY = std::cos(wheelAngleRad);

		// Wheel Velocity
		float angVelRad = Util::ToRad(angularVelocity);
		float wheelVelX = velX + (-angVelRad * wy);
		float wheelVelY = velY + (angVelRad * wx);

		float fVelLong = wheelVelX * wheelDirX + wheelVelY * wheelDirY;
		float fVelLat  = wheelVelX * wheelSideX + wheelVelY * wheelSideY;
		
		// Longitudinal Forces
		float tractionForce = 0.0f;
		if (w.powered && !isOutOfFuel) {
			tractionForce = (throttleInput * ENGINE_POWER * enginePowerMultiplier);
		}
		      
		// Braking / Rolling resistance
		if (std::abs(fVelLong) > 1.0f) {
			float brakeForce = (brakeInput * MAX_BRAKE_FORCE) + ROLLING_RESISTANCE;
			if (brakeForce > std::abs(fVelLong) * mass) brakeForce = std::abs(fVelLong) * mass; // Prevent overshoot
			tractionForce -= (fVelLong > 0 ? 1.0f : -1.0f) * brakeForce;
		}

		// Lateral Forces (Cornering)
		// High-damping slip angle calculation
		float slipAngle = std::atan2(fVelLat, std::abs(fVelLong) + 10.0f);
		float corneringForce = -slipAngle * CORNERING_STIFFNESS;
		
		// Grip limit - apply drift effects to rear wheels
		float maxGrip = 600000.0f;
		if (w.powered) { // Rear wheels
			maxGrip *= rearWheelGripMultiplier;
		}
		if (corneringForce > maxGrip) corneringForce = maxGrip;
		if (corneringForce < -maxGrip) corneringForce = -maxGrip;

		// Integrate Forces
		float wheelForceX = wheelDirX * tractionForce + wheelSideX * corneringForce;
		float wheelForceY = wheelDirY * tractionForce + wheelSideY * corneringForce;

		forceSumX += wheelForceX;
		forceSumY += wheelForceY;
		torqueSum += (wx * wheelForceY - wy * wheelForceX);
	}

	// Drag
    forceSumX -= velX * speed * DRAG_COEFF;
    forceSumY -= velY * speed * DRAG_COEFF;

	// Integrate
	float accelX = forceSumX / mass;
	float accelY = forceSumY / mass;
	
	velX += accelX * dt;
	velY += accelY * dt;
	
    // Angular acceleration
    float angularAccel = torqueSum / inertia;
    // angularVelocity store in degrees per sec for compatibility, but calc in rads?
    // Let's keep angularVelocity in DEGREES/s as declared, so convert accel rad/s^2 -> deg/s^2
    angularVelocity += Util::ToDeg(angularAccel) * dt;

	// Apply velocity damping to help stop at rest
    if (speed < 5.0f && throttleInput == 0.0f) {
        velX = 0;
        velY = 0;
        angularVelocity = 0;
    }

    velX /= (1.0f + 0.3f * dt); // Increased damping from 0.1f to 0.3f
    velY /= (1.0f + 0.3f * dt); // Increased damping from 0.1f to 0.3f
    // angularVelocity /= (1.0f + 0.5f * dt);

	// Apply Move (Standard collision check)
    // We break velocity into steps if needed, or just single step for now
    
	float carDirX = std::cos(angleRad);
	float carDirY = std::sin(angleRad);

	// --- Calculate Body Roll for Visual Effect ---
	// Body roll is caused by lateral acceleration during turns
	// The car leans to the outside of the turn (opposite to turn direction)
	const float ROLL_SENSITIVITY = 0.015f; // How much the car rolls per unit of lateral force
	const float MAX_ROLL_ANGLE = 8.0f; // Maximum roll angle in degrees
	const float ROLL_DAMPING = 8.0f; // How quickly roll returns to neutral
	
	// Calculate lateral acceleration (perpendicular to car direction)
	float carSideX = -std::sin(angleRad);
	float carSideY = std::cos(angleRad);
	
	// Lateral component of acceleration
	float lateralAccel = accelX * carSideX + accelY * carSideY;
	
	// Target roll angle based on lateral acceleration and angular velocity
	// Positive roll = lean right, negative = lean left
	float targetRoll = -lateralAccel * ROLL_SENSITIVITY;
	
	// Also add roll from angular velocity (turning creates centripetal acceleration)
	targetRoll += angularVelocity * 0.05f;
	
	// Enhanced roll during drift for dramatic effect
	if (isDrifting) {
		targetRoll *= 2.0f; // Double the roll during drift
		// Add some extra roll based on drift intensity
		float driftIntensity = (driftDuration - driftTimer) / driftDuration;
		targetRoll += driftIntensity * 5.0f; // Additional roll based on drift duration
	}
	
	// Clamp to maximum roll
	if (targetRoll > MAX_ROLL_ANGLE) targetRoll = MAX_ROLL_ANGLE;
	if (targetRoll < -MAX_ROLL_ANGLE) targetRoll = -MAX_ROLL_ANGLE;
	
	// Smoothly interpolate current roll towards target
	float rollDiff = targetRoll - bodyRoll;
	bodyRoll += rollDiff * ROLL_DAMPING * dt;
	
	// Snap to zero if very close
	if (std::abs(bodyRoll) < 0.01f) bodyRoll = 0.0f;

    // Convert velocity to legacy "moveSpeed" for compatibility/display
    moveSpeed = speed * ( (velX*carDirX + velY*carDirY) > 0 ? 1.f : -1.f );

    // --- Collision Logic ---
    // Use multiple points around the car for more accurate collision handling
    struct CollisionPoint { float lx, ly; };
    const float bodyLength = 32.0f;
    const float bodyWidth = 16.0f;
    
    std::vector<CollisionPoint> collPoints = {
        { bodyLength,  bodyWidth}, { bodyLength, -bodyWidth},
        {-bodyLength,  bodyWidth}, {-bodyLength, -bodyWidth},
        { bodyLength,   0.0f}, {-bodyLength,   0.0f},
        { 0.0f,  bodyWidth}, { 0.0f, -bodyWidth}
    };

    bool collided = false;
    float angVelRad = Util::ToRad(angularVelocity);

    for (const auto& p : collPoints) {
        // Current world position of this point
        float qx = p.lx * std::cos(angleRad) - p.ly * std::sin(angleRad);
        float qy = p.lx * std::sin(angleRad) + p.ly * std::cos(angleRad);
        float worldPX = posX + qx;
        float worldPY = posY + qy;

        // Velocity of this point (linear + angular)
        float pVelX = velX + (-angVelRad * qy);
        float pVelY = velY + (angVelRad * qx);

        // Next position of this point
        float nextPX = worldPX + pVelX * dt;
        float nextPY = worldPY + pVelY * dt;

        if (!(flags & EntFlag_UnderAllOtherEnts))
        {
            for (auto cl : hostMap->collLines) {
                if (Util::Intersects(worldPX, worldPY, nextPX, nextPY, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y)) {
                    collided = true;

                    // Wall normal
                    float dx = cl->p2.x - cl->p1.x;
                    float dy = cl->p2.y - cl->p1.y;
                    float wallLen = std::sqrt(dx * dx + dy * dy);
                    if (wallLen < 0.001f) continue;
                    float nx = -dy / wallLen;
                    float ny = dx / wallLen;

                    // Ensure normal points away from the wall towards the car center
                    // Use the average of wall points as a reference for "inside"
                    float wallCenterX = (cl->p1.x + cl->p2.x) * 0.5f;
                    float wallCenterY = (cl->p1.y + cl->p2.y) * 0.5f;
                    if (nx * (posX - wallCenterX) + ny * (posY - wallCenterY) < 0) {
                        nx = -nx;
                        ny = -ny;
                    }

                    float vDotN = pVelX * nx + pVelY * ny;
                    if (vDotN < 0) {
                        // Collision Response (Impulse)
                        float restitution = 0.1f; // Lower restitution for less bouncy feel
                        float j = -(1.0f + restitution) * vDotN;
                        
                        float rCrossN = qx * ny - qy * nx;
                        float impulse = j / ((1.0f / mass) + (rCrossN * rCrossN / inertia));

                        velX += (impulse * nx) / mass;
                        velY += (impulse * ny) / mass;
                        float dAngularVel = (rCrossN * impulse) / inertia;
                        angularVelocity += Util::ToDeg(dAngularVel);

                        // Friction with wall
                        velX *= 0.95f;
                        velY *= 0.95f;
                        angularVelocity *= 0.95f;

                        if (!collisionSoundPlayed && speed > 100.0f) {
                            Sfx::WallHit->play(posX, posY);
                            collisionSoundPlayed = true;

                                if (auto actor = dynamic_cast<Actor*>(occupant)) {
                                    // Damage is proportional to the change in velocity (deltaV)
                                    float deltaV = impulse / mass;
                                    float damage = deltaV * 0.06f; 
                                    if (damage > 0.5f) {
                                        actor->hurt(damage, Actor::DamageReason_VehicleCollision, nullptr);
                                    }
                                }
                        }
                    }

                    // Simple ejection to prevent clipping
                    posX += nx * 0.5f;
                    posY += ny * 0.5f;
                    
                    break;
                }
            }
        }
    }

    posX += velX * dt;
    posY += velY * dt;

    // --- Robust OBB-based wall collision 'catch-all' ---
    // This prevents the car from 'tunneling' through thin collision lines
    if (!(flags & EntFlag_UnderAllOtherEnts))
    {
        OBB* currentOBB = getOBB();
        // Check for intersection with all map collision lines
        for (auto cl : hostMap->collLines) {
            Point mtv;
            if (OBBIntersectsLine(*currentOBB, cl->p1.x, cl->p1.y, cl->p2.x, cl->p2.y, &mtv)) {
                collided = true;
                
                // Position correction (push out of wall)
                posX += mtv.x;
                posY += mtv.y;
                currentOBB = getOBB(); // Update OBB for next line check

                // Velocity response
                float mtvLen = std::sqrt(mtv.x * mtv.x + mtv.y * mtv.y);
                if (mtvLen > 0.0001f) {
                    Point normal = { mtv.x / mtvLen, mtv.y / mtvLen };
                    float vDotN = velX * normal.x + velY * normal.y;
                    
                    if (vDotN < 0) {
                        float restitution = 0.2f;
                        float j = -(1.0f + restitution) * vDotN;
                        
                        // Apply impulse
                        velX += normal.x * j;
                        velY += normal.y * j;
                        
                        // Friction
                        velX *= 0.98f;
                        velY *= 0.98f;
                        angularVelocity *= 0.95f;
                        
                        if (!collisionSoundPlayed && speed > 50.0f) {
                            Sfx::WallHit->play(posX, posY);
                            collisionSoundPlayed = true;
                        }
                    }
                }
            }
        }
    }

    // Check collisions with entities (actors)
    float carSpeed = std::sqrt(velX * velX + velY * velY);
    OBB* carOBB = getOBB();
    for (auto e : hostMap->getEntities()) {
        if (!e || e == this || (e->flags & EntFlag_Dead) || (e->flags & EntFlag_UnderAllOtherEnts)) continue;
        if (auto actor = dynamic_cast<Actor*>(e)) {
            OBB* actorOBB = actor->getOBB();
            if (OBBIntersects(*carOBB, *actorOBB)) {
                collided = true; // Mark as collided for stuck detection
                
                // Hurt the actor
                float damage = 3.0f * (carSpeed / 37.0f);
                if (damage > 0.1f) actor->hurt(damage, Actor::DamageReason_VehicleCollision, this);
                
                // Launch in direction of car's velocity, or away from center if slow
                float launchAngle;
                if (carSpeed > 5.0f) {
                    launchAngle = Util::ToDeg(std::atan2(velY, velX));
                } else {
                    launchAngle = Util::ToDeg(std::atan2(actor->posY - posY, actor->posX - posX));
                }
                
                actor->physicsSlideAngle = launchAngle;
                // Minimum launch amount to prevent getting snagged/tangled
                actor->physicsSlideAmount = std::max(15.0f, carSpeed * 2.5f);
                
                // Launch actor upward based on car speed
                actor->velZ = std::max(8.0f, carSpeed * 0.35f); 
                
                if (!collisionSoundPlayed && carSpeed > 30.0f) {
                    Sfx::WallHit->play(actor->posX, actor->posY);
                    collisionSoundPlayed = true;
                }
            }
        }
        else if (auto wall = dynamic_cast<BuildingWallEntity*>(e)) {
            // Check collision with physics-enabled building wall
            OBB* wallOBB = wall->getOBB();
            Point mtv;
            if (OBBIntersects(*carOBB, *wallOBB, &mtv)) {
                collided = true;
                
                // Separate the car from the wall to prevent teleportation
                posX += mtv.x;
                posY += mtv.y;
                
                // Calculate impact force (avoid division by zero)
                float impactForce = carSpeed * mass;
                
                // Find contact point (approximate center of overlap)
                Point wallCenter = wallOBB->center;
                
                // Apply push force to the wall
                float pushDirX, pushDirY;
                if (carSpeed > 1.0f) {
                    pushDirX = velX / carSpeed;
                    pushDirY = velY / carSpeed;
                } else {
                    // If not moving, push away from wall center
                    pushDirX = (float)(posX - wallCenter.x);
                    pushDirY = (float)(posY - wallCenter.y);
                    float len = std::sqrt(pushDirX * pushDirX + pushDirY * pushDirY);
                    if (len > 0.001f) {
                        pushDirX /= len;
                        pushDirY /= len;
                    }
                }
                wall->applyPushForce(pushDirX, pushDirY, impactForce, wallCenter.x, wallCenter.y);
                
                // Also apply damage to the wall
                float damage = carSpeed * 0.15f;
                if (damage > 1.0f) {
                    wall->hurt(damage, DamageReason_VehicleCollision, this);
                }
                
                // Bounce the car back slightly (avoid division by zero)
                float bounceFactor = 0.3f;
                if (carSpeed > 1.0f) {
                    velX -= pushDirX * carSpeed * bounceFactor;
                    velY -= pushDirY * carSpeed * bounceFactor;
                }
                
                // Play collision sound
                if (!collisionSoundPlayed && carSpeed > 40.0f) {
                    Sfx::WallHit->play(wall->posX, wall->posY);
                    collisionSoundPlayed = true;
                }
            }
        }
    }

    if (!collided) {
        collisionSoundPlayed = false;
    }
    lastFrameCollided = collided;
}

bool CarBase::autoDriveTo(float targetX, float targetY)
{
    // If we're already driving to this exact target, don't reset timers!
    if (isAutoDriving && std::abs(autoDriveTargetX - targetX) < 0.1f && std::abs(autoDriveTargetY - targetY) < 0.1f) {
        return isAutoDriving;
    }

    isAutoDriving = true;
    autoDriveTargetX = targetX;
    autoDriveTargetY = targetY;
    autoDriveStuckTimer = 0.0f;
    autoDriveReverseTimer = 0.0f;
    isAutoDriveReversing = false;
    reSolveAST = true;
    directPath = false;
    
    float dist = Util::Dist(posX, posY, targetX, targetY);
    if (dist < 50.0f) {
        stopAutoDrive();
        return false;
    }
    return true;
}

void CarBase::stopAutoDrive()
{
    isAutoDriving = false;
    autoDriveStuckTimer = 0.0f;
    autoDriveReverseTimer = 0.0f;
    isAutoDriveReversing = false;
    if (ast) {
        delete ast;
        ast = nullptr;
    }
    astn = nullptr;
}

void CarBase::hurt(float amount, DamageReason damageReason, Entity* damageSource)
{
    health -= amount;
    if (health < 0) health = 0;
}

} // namespace nyaa
