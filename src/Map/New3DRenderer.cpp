#include "Map/New3DRenderer.h"
#include <SFML/OpenGL.hpp>
// #include <rlgl.h>
#include "Map/Map.h"
#include "Game/Mouse.h"
#include "Entity/BuildingWallEntity.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif
#include "Map/MapChunk.h"
#include "Map/Pathfinding.h"
#include "Texture/Sheet.h"
#include "Screen/InGameScreen.h"
#include "Entity/Actor.h"
#include "Entity/Prop.h"
#include "Entity/BulletProjectile.h"
#include "Entity/CarBase.h"
#include "Entity/DevCar.h"
#include "Texture/WalkAnim.h"
#include "Inventory/Gun.h"
#include "Game/Util.h"
#include <GL/glu.h>
#include <cmath>
#include <algorithm>
#include "Game/Game.h"
#include "Shader/SphericalAberrationShader.h"
#include "Shader/New3DMainShader.h"
#include <limits>
#include "Entity/Light.h"
#include "Entity/Road.h"
#include "Map/Building.h"
#include "Game/GameClock.h"

typedef void (WINAPI *PFNGLUSEPROGRAMPROC)(GLuint program);
static PFNGLUSEPROGRAMPROC glUseProgram = nullptr;

namespace nyaa {

New3DRenderer::New3DRenderer()
    :particleOpacity(0.0f)
    ,m_new3DMainShader(new New3DMainShader())
    ,wavyWorldManager(new DrugEffectsManager())
    ,worldVibrationManager(new DrugEffectsManager())
    ,colorShiftManager(new DrugEffectsManager())
    ,kaleidoscopeManager(new DrugEffectsManager())
    ,noiseManager(new DrugEffectsManager())
    ,greyscaleManager(new DrugEffectsManager())
    ,stretchManager(new DrugEffectsManager())
    ,bitcrushManager(new DrugEffectsManager())
    ,worldScramblerManager(new DrugEffectsManager())
    ,fractalManager(new DrugEffectsManager())
    ,depthPerceptionManager(new DrugEffectsManager())
    ,dissolveManager(new DrugEffectsManager())
    ,rgbDeconvergenceManager(new DrugEffectsManager())
    ,radialBlurManager(new DrugEffectsManager())
    ,bloomManager(new DrugEffectsManager())
    ,fullbrightEnabled(false)
{
    glUseProgram = (PFNGLUSEPROGRAMPROC)(void(*)())wglGetProcAddress("glUseProgram");
    
    // Create white texture for untextured rendering
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    unsigned char white[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

New3DRenderer::~New3DRenderer()
{
    delete m_new3DMainShader;
    
    // Clean up drug effects managers
    delete wavyWorldManager;
    delete worldVibrationManager;
    delete colorShiftManager;
    delete kaleidoscopeManager;
    delete noiseManager;
    delete greyscaleManager;
    delete stretchManager;
    delete bitcrushManager;
    delete worldScramblerManager;
    delete fractalManager;
    delete depthPerceptionManager;
    delete dissolveManager;
    delete rgbDeconvergenceManager;
    delete radialBlurManager;
    delete bloomManager;
}

void New3DRenderer::render(Map* map, RendTarget& target)
{
    target.pushGLStates();
    
    // Update all drug effects managers
    float deltaTime = G->frameDeltaMillis / 1000.0f;
    wavyWorldManager->update(deltaTime);
    worldVibrationManager->update(deltaTime);
    colorShiftManager->update(deltaTime);
    kaleidoscopeManager->update(deltaTime);
    noiseManager->update(deltaTime);
    greyscaleManager->update(deltaTime);
    stretchManager->update(deltaTime);
    bitcrushManager->update(deltaTime);
    worldScramblerManager->update(deltaTime);
    fractalManager->update(deltaTime);
    depthPerceptionManager->update(deltaTime);
    dissolveManager->update(deltaTime);
    rgbDeconvergenceManager->update(deltaTime);
    radialBlurManager->update(deltaTime);
    bloomManager->update(deltaTime);
    
    if (m_new3DMainShader) {
        m_new3DMainShader->setTime(shaderClock.getElapsedSeconds());
        m_new3DMainShader->setDrugEffect(wavyWorldManager->getCurrentEffect());
        m_new3DMainShader->setWorldVibration(worldVibrationManager->getCurrentEffect());
        m_new3DMainShader->setColorShift(colorShiftManager->getCurrentEffect());
        m_new3DMainShader->setKaleidoscopeEffect(kaleidoscopeManager->getCurrentEffect());
        m_new3DMainShader->setNoiseEffect(noiseManager->getCurrentEffect());
        m_new3DMainShader->setGreyscaleEffect(greyscaleManager->getCurrentEffect());
        m_new3DMainShader->setStretchEffect(stretchManager->getCurrentEffect());
        m_new3DMainShader->setBitcrushEffect(bitcrushManager->getCurrentEffect());
        m_new3DMainShader->setWorldScramblerEffect(worldScramblerManager->getCurrentEffect());
        m_new3DMainShader->setPlayerPos(IGS->player->posX, IGS->player->posY);
        m_new3DMainShader->setFractalDrugEffect(fractalManager->getCurrentEffect());
        m_new3DMainShader->setDepthPerceptionEffect(depthPerceptionManager->getCurrentEffect());
        m_new3DMainShader->setDissolveEffect(dissolveManager->getCurrentEffect());
        m_new3DMainShader->setDaytime(map->type == Map::Type_Outdoors ? IGS->gameClock->getTimeOfDay() : 0.f);
        m_new3DMainShader->setFog(0.005f, 20.0f, 0.6f, 0.7f, 0.8f); // Distance-based fog starting at 20 tiles

        // Set camera position for fog calculations (camera is at origin after world translation)
        m_new3DMainShader->setCameraPosition(0.0f, 0.0f, 0.0f);

        updateLighting(map);

        m_new3DMainShader->update();

        GLuint programId = m_new3DMainShader->shader.getNativeHandle();
        if (programId && glUseProgram) {
            glUseProgram(programId);
        }
    }
    
    setupCamera(target);
    
    renderFloorTiles(map, target);
    
    renderPortals(map);

    renderRoads(map, target);
    renderActorShadows(map, target);
    renderBuildings(map, target);

    renderUrinePuddles(map);

#if DEBUG
    renderCollisionDebug(map, target);
    renderVelocityDebug(map, target);

    renderTileEditHighlights(map, target);
#endif
    renderObjects(map, target);

    // Render debug text above actors
    renderDebugTextAboveActors(map, target);
    
    if(IGS->showEntityOBBDebug) renderEntityOBBDebug(map, target);

    renderAutoWalkParticles();

    renderCeilingTiles(map, target);

    renderRain3D();

    // Update mouse raycasting for normal vectors
    updateMouseRaycast(map, target);
    
    // Render normal vector arrow if mouse is over a wall
    if (currentWallHit.hit) {
        renderNormalVectorArrow(map, target);
    }

    if (glUseProgram) {
        glUseProgram(0);
    }

    glDisable(GL_DEPTH_TEST);
    
    target.popGLStates();
}

void New3DRenderer::updateLighting(Map* map)
{
    if (!m_new3DMainShader) return;

    // Clear all lights and rebuild from entities
    m_new3DMainShader->clearLights();

    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    // Add lights from Light entities in the map
    for (Entity* entity : map->getEntities()) {
        Light* lightEntity = dynamic_cast<Light*>(entity);
        if (lightEntity) {
            auto& shaderLight = m_new3DMainShader->addLight();
            // Convert 2D position to 3D (x, height, z) relative to camera (translated coordinates)
            // Use double precision for accurate calculations at large distances
            shaderLight.positionX = lightEntity->posX / 16.0 - camWorldX;
            shaderLight.positionY = 2.0;                        // Fixed height
            shaderLight.positionZ = lightEntity->posY / 16.0 - camWorldZ;

            shaderLight.colorR = lightEntity->colorR / 255.0f;
            shaderLight.colorG = lightEntity->colorG / 255.0f;
            shaderLight.colorB = lightEntity->colorB / 255.0f;
            shaderLight.intensity = lightEntity->intensity;
        }
    }

    // Add headlights from cars
    m_new3DMainShader->clearHeadlights();
    int carsProcessed = 0;
    // Determine if headlights should be on based on time of day (for outdoor maps)
    float timeOfDay = IGS->gameClock->getTimeOfDay();
    bool isNight = (timeOfDay < 6.6f/24.0f || timeOfDay > 20.0f/24.0f);
    bool lightsShouldBeOn = (map->type == Map::Type_Outdoors ? isNight : true);

    for (Entity* entity : map->getEntities()) {
        CarBase* car = dynamic_cast<CarBase*>(entity);
        if (car && (car->occupant || car->isAutoDriving) && !car->isOutOfFuel && lightsShouldBeOn) {
            if (carsProcessed >= 16) break;

            double rad = Util::ToRad(car->dirAngle);
            double forwardX = std::cos(rad);
            double forwardY = std::sin(rad);
            double sideX = -forwardY;
            double sideY = forwardX;

            double frontDist = 28.0 / 16.0; 
            double sideDist = 12.0 / 16.0;
            double height = 0.5;
            
            double tiltDown = -0.15; // Angled slightly down to hit the ground
            double dirLen = std::sqrt(1.0 + tiltDown * tiltDown);
            double hlDirX = forwardX / dirLen;
            double hlDirY = tiltDown / dirLen;
            double hlDirZ = forwardY / dirLen;

            // Headlight 1 (Right)
            New3DMainShader::Headlight hl1;
            // Use double precision for accurate headlight positioning at large distances
            hl1.positionX = (car->posX / 16.0 - camWorldX) + forwardX * frontDist + sideX * sideDist;
            hl1.positionY = height;
            hl1.positionZ = (car->posY / 16.0 - camWorldZ) + forwardY * frontDist + sideY * sideDist;
            hl1.directionX = hlDirX;
            hl1.directionY = hlDirY;
            hl1.directionZ = hlDirZ;
            hl1.colorR = 1.0f; hl1.colorG = 1.0f; hl1.colorB = 0.9f;
            hl1.intensity = 2.5f;
            m_new3DMainShader->addHeadlight(hl1);

            // Headlight 2 (Left)
            New3DMainShader::Headlight hl2;
            // Use double precision for accurate headlight positioning at large distances
            hl2.positionX = (car->posX / 16.0 - camWorldX) + forwardX * frontDist - sideX * sideDist;
            hl2.positionY = height;
            hl2.positionZ = (car->posY / 16.0 - camWorldZ) + forwardY * frontDist - sideY * sideDist;
            hl2.directionX = hlDirX;
            hl2.directionY = hlDirY;
            hl2.directionZ = hlDirZ;
            hl2.colorR = 1.0f; hl2.colorG = 1.0f; hl2.colorB = 0.9f;
            hl2.intensity = 2.5f;
            m_new3DMainShader->addHeadlight(hl2);

            carsProcessed++;
        }
    }
}

void New3DRenderer::renderRoads(Map* map, RendTarget& target)
{
    std::vector<Road*> roads;
    for (Entity* entity : map->getEntities()) {
        Road* road = dynamic_cast<Road*>(entity);
        if (road && !road->anchors.empty()) {
            roads.push_back(road);
        }
    }

    if (roads.empty()) return;

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    for (Road* road : roads) {
        RoadMesh mesh = road->generateCatmullRoadMesh();
        
        // Asphalt triangles
        glColor4ub(road->asphaltColorR, road->asphaltColorG, road->asphaltColorB, 255);
        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < mesh.indices.size(); ++i) {
            const RoadVertex& v = mesh.vertices[mesh.indices[i]];
            glTexCoord2f(v.texCoordX, v.texCoordY);
            glNormal3f(v.normal.x, v.normal.y, v.normal.z);
            // Use double precision for accurate positioning at large distances
            glVertex3f((float)(v.position.x / 16.0 - camWorldX), 0.05f, (float)(v.position.z / 16.0 - camWorldZ));
        }
        glEnd();
        
        // Asphalt circles at anchors to smooth joints
        for (const auto& anchor : road->anchors) {
            // Use double precision for accurate anchor positioning at large distances
            double x = anchor.x - camWorldX;
            double z = anchor.y - camWorldZ;
            float radius = road->width * 0.5f / 16.0f;
            glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(0.5f, 0.5f);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f((float)x, 0.06f, (float)z);
            for (int i = 0; i <= 16; ++i) {
                float angle = i * 3.14159f * 2.0f / 16.0f;
                glTexCoord2f(0.5f + cos(angle) * 0.5f, 0.5f + sin(angle) * 0.5f);
                glVertex3f((float)(x + cos(angle) * radius), 0.06f, (float)(z + sin(angle) * radius));
            }
            glEnd();
        }
        
        // Center lines as triangles
        glColor4ub(road->linesColorR, road->linesColorG, road->linesColorB, 255);
        float lineWidth = 0.08f;
        glBegin(GL_TRIANGLES);
        if(mesh.vertices.size() >= 4) for (size_t i = 0; i < mesh.vertices.size() - 2; i += 2) {
            const RoadVertex& left1 = mesh.vertices[i];
            const RoadVertex& right1 = mesh.vertices[i + 1];
            const RoadVertex& left2 = mesh.vertices[i + 2];
            const RoadVertex& right2 = mesh.vertices[i + 3];
            
            // Use double precision for accurate center line calculations at large distances
            double centerX1 = (left1.position.x + right1.position.x) * 0.5 / 16.0 - camWorldX;
            double centerZ1 = (left1.position.z + right1.position.z) * 0.5 / 16.0 - camWorldZ;
            double centerX2 = (left2.position.x + right2.position.x) * 0.5 / 16.0 - camWorldX;
            double centerZ2 = (left2.position.z + right2.position.z) * 0.5 / 16.0 - camWorldZ;
            
            float dx = centerX2 - centerX1;
            float dz = centerZ2 - centerZ1;
            float len = sqrt(dx*dx + dz*dz);
            if (len > 0.0001f) {
                dx /= len; dz /= len;
                float perpX = -dz * lineWidth;
                float perpZ = dx * lineWidth;
                
                glNormal3f(0.0f, 1.0f, 0.0f);
                
                glTexCoord2f(0.5f, 0.0f); glVertex3f(centerX1 - perpX, 0.07f, centerZ1 - perpZ);
                glTexCoord2f(0.5f, 1.0f); glVertex3f(centerX1 + perpX, 0.07f, centerZ1 + perpZ);
                glTexCoord2f(0.5f, 0.0f); glVertex3f(centerX2 - perpX, 0.07f, centerZ2 - perpZ);
                
                glTexCoord2f(0.5f, 1.0f); glVertex3f(centerX1 + perpX, 0.07f, centerZ1 + perpZ);
                glTexCoord2f(0.5f, 1.0f); glVertex3f(centerX2 + perpX, 0.07f, centerZ2 + perpZ);
                glTexCoord2f(0.5f, 0.0f); glVertex3f(centerX2 - perpX, 0.07f, centerZ2 - perpZ);
            }
        }
        glEnd();

        if (IGS->roadEditOn) {
            glDisable(GL_TEXTURE_2D);
            for (size_t i = 0; i < road->anchors.size(); ++i) {
                const auto& anchor = road->anchors[i];
                // Use double precision for accurate anchor positioning at large distances
                double x = anchor.x;
                double z = anchor.y;
                float radius = 0.2f;
                
                if (road == IGS->lastEditedRoad) {
                    // Highlight selected road anchors
                    glColor3f(1.0f, 1.0f, 0.0f);
                    radius = 0.3f;
                } else {
                    glColor3f(0.7f, 0.7f, 0.7f);
                }
                
                glBegin(GL_TRIANGLE_FAN);
                glVertex3f((float)x, 0.05f, (float)z);
                for (int j = 0; j <= 8; ++j) {
                    float angle = j * 3.14159f * 2.0f / 8.0f;
                    glVertex3f((float)(x + std::cos(angle) * radius), 0.05f, (float)(z + std::sin(angle) * radius));
                }
                glEnd();
            }
            glEnable(GL_TEXTURE_2D);
        }
    }

    glDisable(GL_MULTISAMPLE);
}

void New3DRenderer::renderBuildings(Map* map, RendTarget& target)
{
    if (map->buildings.empty()) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    
    // Enable alpha blending for potential transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;

    glBegin(GL_QUADS);

    for (Building* b : map->buildings) {
		// Skip rendering static walls for buildings using physics walls
        // (they're rendered as BuildingWallEntity objects instead)
        if (b->usePhysicsWalls) continue;
        
        float currentBaseY = 0.0f; // Start at ground level

        for (Floor* f : b->floors) {
            float floorHeight = f->height / 16.0f;
            float topY = currentBaseY + floorHeight;

            for (BuildingWall& w : f->walls) {
                // Calculate wall coordinates relative to camera
                double wX1 = w.x1 / 16.0 - camWorldX;
                double wZ1 = w.y1 / 16.0 - camWorldZ;
                double wX2 = w.x2 / 16.0 - camWorldX;
                double wZ2 = w.y2 / 16.0 - camWorldZ;

                // Wall direction
                double dx = wX2 - wX1;
                double dz = wZ2 - wZ1;
                double len = sqrt(dx*dx + dz*dz);
                
                if (len < 0.001) continue;

                // Perpendicular vector for thickness
                double thickness = 4.0 / 16.0; // 4 pixels thickness
                double px = (-dz / len) * (thickness * 0.5);
                double pz = (dx / len) * (thickness * 0.5);

                // 4 corners of the base footprint
                // Start point side
                double p1x = wX1 + px; double p1z = wZ1 + pz;
                double p2x = wX1 - px; double p2z = wZ1 - pz;
                
                // End point side
                double p3x = wX2 - px; double p3z = wZ2 - pz;
                double p4x = wX2 + px; double p4z = wZ2 + pz;

                // Wall Color (Light Gray for now)
                glColor3f(0.85f, 0.85f, 0.85f); 

                // --- Vertical Faces ---
                
                // Side 1 (Left when looking from start to end) - p4 to p1
                glNormal3f((float)(px/thickness*2), 0, (float)(pz/thickness*2));
                glTexCoord2f(0.0f, 0.0f); glVertex3f((float)p4x, currentBaseY, (float)p4z);
                glTexCoord2f(len, 0.0f);  glVertex3f((float)p1x, currentBaseY, (float)p1z);
                glTexCoord2f(len, 1.0f);  glVertex3f((float)p1x, topY, (float)p1z);
                glTexCoord2f(0.0f, 1.0f); glVertex3f((float)p4x, topY, (float)p4z);

                // Side 2 (Right) - p2 to p3
                glNormal3f((float)(-px/thickness*2), 0, (float)(-pz/thickness*2));
                glTexCoord2f(0.0f, 0.0f); glVertex3f((float)p2x, currentBaseY, (float)p2z);
                glTexCoord2f(len, 0.0f);  glVertex3f((float)p3x, currentBaseY, (float)p3z);
                glTexCoord2f(len, 1.0f);  glVertex3f((float)p3x, topY, (float)p3z);
                glTexCoord2f(0.0f, 1.0f); glVertex3f((float)p2x, topY, (float)p2z);

                // End caps
                // Start Cap (p1 to p2)
                glNormal3f((float)(-dx/len), 0, (float)(-dz/len));
                glTexCoord2f(0,0); glVertex3f((float)p1x, currentBaseY, (float)p1z);
                glTexCoord2f(1,0); glVertex3f((float)p2x, currentBaseY, (float)p2z);
                glTexCoord2f(1,1); glVertex3f((float)p2x, topY, (float)p2z);
                glTexCoord2f(0,1); glVertex3f((float)p1x, topY, (float)p1z);

                // End Cap (p3 to p4)
                glNormal3f((float)(dx/len), 0, (float)(dz/len));
                glTexCoord2f(0,0); glVertex3f((float)p3x, currentBaseY, (float)p3z);
                glTexCoord2f(1,0); glVertex3f((float)p4x, currentBaseY, (float)p4z);
                glTexCoord2f(1,1); glVertex3f((float)p4x, topY, (float)p4z);
                glTexCoord2f(0,1); glVertex3f((float)p3x, topY, (float)p3z);

                // Top Face
                glColor3f(0.75f, 0.75f, 0.75f);
                glNormal3f(0, 1, 0);
                glTexCoord2f(0,0); glVertex3f((float)p1x, topY, (float)p1z);
                glTexCoord2f(0,1); glVertex3f((float)p2x, topY, (float)p2z);
                glTexCoord2f(1,1); glVertex3f((float)p3x, topY, (float)p3z);
                glTexCoord2f(1,0); glVertex3f((float)p4x, topY, (float)p4z);
            }

            currentBaseY += floorHeight;
        }
    }
    glEnd();
}

// Public methods to add drug effects
void New3DRenderer::addWavyWorldEffect(float effectValue, float durationSeconds)
{
    wavyWorldManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addWorldVibrationEffect(float effectValue, float durationSeconds)
{
    worldVibrationManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addColorShiftEffect(float effectValue, float durationSeconds)
{
    colorShiftManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addKaleidoscopeEffect(float effectValue, float durationSeconds)
{
    kaleidoscopeManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addNoiseEffect(float effectValue, float durationSeconds)
{
    noiseManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addGreyscaleEffect(float effectValue, float durationSeconds)
{
    greyscaleManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addStretchEffect(float effectValue, float durationSeconds)
{
    stretchManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addBitcrushEffect(float effectValue, float durationSeconds)
{
    bitcrushManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addWorldScramblerEffect(float effectValue, float durationSeconds)
{
    worldScramblerManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addFractalEffect(float effectValue, float durationSeconds)
{
    fractalManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addDepthPerceptionEffect(float effectValue, float durationSeconds)
{
    depthPerceptionManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addDissolveEffect(float effectValue, float durationSeconds)
{
    dissolveManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addRGBDeconvergenceEffect(float effectValue, float durationSeconds)
{
    rgbDeconvergenceManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addRadialBlurEffect(float effectValue, float durationSeconds)
{
    radialBlurManager->addEffect(effectValue, durationSeconds);
}

void New3DRenderer::addBloomEffect(float effectValue, float durationSeconds)
{
    bloomManager->addEffect(effectValue, durationSeconds);
}

// Getter methods for accessing current effect values
float New3DRenderer::getCurrentWavyWorldEffect() const
{
    return wavyWorldManager ? wavyWorldManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentWorldVibrationEffect() const
{
    return worldVibrationManager ? worldVibrationManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentColorShiftEffect() const
{
    return colorShiftManager ? colorShiftManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentKaleidoscopeEffect() const
{
    return kaleidoscopeManager ? kaleidoscopeManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentNoiseEffect() const
{
    return noiseManager ? noiseManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentGreyscaleEffect() const
{
    return greyscaleManager ? greyscaleManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentStretchEffect() const
{
    return stretchManager ? stretchManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentBitcrushEffect() const
{
    return bitcrushManager ? bitcrushManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentWorldScramblerEffect() const
{
    return worldScramblerManager ? worldScramblerManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentFractalEffect() const
{
    return fractalManager ? fractalManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentDepthPerceptionEffect() const
{
    return depthPerceptionManager ? depthPerceptionManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentDissolveEffect() const
{
    return dissolveManager ? dissolveManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentRGBDeconvergenceEffect() const
{
    return rgbDeconvergenceManager ? rgbDeconvergenceManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentRadialBlurEffect() const
{
    return radialBlurManager ? radialBlurManager->getCurrentEffect() : 0.0f;
}

float New3DRenderer::getCurrentBloomEffect() const
{
    return bloomManager ? bloomManager->getCurrentEffect() : 0.0f;
}

// Getter methods for accessing current tolerance values
float New3DRenderer::getCurrentWavyWorldTolerance() const
{
    return wavyWorldManager ? wavyWorldManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentWorldVibrationTolerance() const
{
    return worldVibrationManager ? worldVibrationManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentColorShiftTolerance() const
{
    return colorShiftManager ? colorShiftManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentKaleidoscopeTolerance() const
{
    return kaleidoscopeManager ? kaleidoscopeManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentNoiseTolerance() const
{
    return noiseManager ? noiseManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentGreyscaleTolerance() const
{
    return greyscaleManager ? greyscaleManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentStretchTolerance() const
{
    return stretchManager ? stretchManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentBitcrushTolerance() const
{
    return bitcrushManager ? bitcrushManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentWorldScramblerTolerance() const
{
    return worldScramblerManager ? worldScramblerManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentFractalTolerance() const
{
    return fractalManager ? fractalManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentDepthPerceptionTolerance() const
{
    return depthPerceptionManager ? depthPerceptionManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentDissolveTolerance() const
{
    return dissolveManager ? dissolveManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentRGBDeconvergenceTolerance() const
{
    return rgbDeconvergenceManager ? rgbDeconvergenceManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentRadialBlurTolerance() const
{
    return radialBlurManager ? radialBlurManager->getCurrentTolerance() : 0.0f;
}

float New3DRenderer::getCurrentBloomTolerance() const
{
    return bloomManager ? bloomManager->getCurrentTolerance() : 0.0f;
}

void New3DRenderer::updateMouseRaycast(Map* map, RendTarget& target)
{
    currentWallHit = WallHitInfo(); // Reset hit info
    
    // Get mouse screen coordinates
    int mouseX = Mouse::Pos_X;
    int mouseY = Mouse::Pos_Y;
    
    // Convert screen coordinates to world coordinates
    double worldX, worldY;
    screenToWorld(mouseX, mouseY, target, &worldX, &worldY);
    
    // Get camera position for ray origin (correct eye position)
    const auto& eye = m_camera.getEyePos();
    double rayOriginX = eye.x;
    double rayOriginY = eye.z; // World Y is GL Z
    double rayOriginZ = eye.y; // World height is GL Y
    
    // Calculate ray direction from camera towards the world point on the ground
    // worldX/Y are in world pixels, convert to tiles (GL units)
    double rayDirX = (worldX / 16.0) - rayOriginX;
    double rayDirY = (worldY / 16.0) - rayOriginY;
    double rayDirZ = 0.0 - rayOriginZ; // Target is at ground height (0)
    
    // Normalize ray direction
    double rayLength = std::sqrt(rayDirX * rayDirX + rayDirY * rayDirY + rayDirZ * rayDirZ);
    if (rayLength > 0.0) {
        rayDirX /= rayLength;
        rayDirY /= rayLength;
        rayDirZ /= rayLength;
    }
    
    // Perform raycasting to find wall intersection
    raycastToWall(map, rayOriginX, rayOriginY, rayOriginZ, rayDirX, rayDirY, rayDirZ, currentWallHit);
}

bool New3DRenderer::raycastToWall(Map* map, double rayOriginX, double rayOriginY, double rayOriginZ, 
                                   double rayDirX, double rayDirY, double rayDirZ, WallHitInfo& hitInfo)
{
    double closestDistance = std::numeric_limits<double>::max();
    bool foundHit = false;
    
    // Check all BuildingWallEntity objects
    for (Entity* entity : map->getEntities()) {
        BuildingWallEntity* wall = dynamic_cast<BuildingWallEntity*>(entity);
        if (!wall) continue;
        
        // Get wall properties - Convert to Tile coordinates (from pixels)
        double x1 = wall->getX1() / 16.0;
        double y1 = wall->getY1() / 16.0;
        double x2 = wall->getX2() / 16.0;
        double y2 = wall->getY2() / 16.0;
        double height = wall->getHeight() / 16.0;
        
        // Wall direction vector
        double wallDirX = x2 - x1;
        double wallDirY = y2 - y1;
        double wallLength = std::sqrt(wallDirX * wallDirX + wallDirY * wallDirY);
        
        if (wallLength < 0.001) continue;
        
        // Normalize wall direction
        wallDirX /= wallLength;
        wallDirY /= wallLength;
        
        // Wall normal (perpendicular to wall direction)
        double wallNormalX = -wallDirY;
        double wallNormalY = wallDirX;
        
        // Ray-plane intersection for the wall plane
        // Plane equation: (P - P0) · N = 0, where P0 is a point on the plane, N is normal
        double wallPointX = (x1 + x2) * 0.5;
        double wallPointY = (y1 + y2) * 0.5;
        
        double denominator = rayDirX * wallNormalX + rayDirY * wallNormalY;
        if (std::abs(denominator) < 0.0001) continue; // Ray is parallel to wall
        
        double t = ((wallPointX - rayOriginX) * wallNormalX + (wallPointY - rayOriginY) * wallNormalY) / denominator;
        
        if (t < 0.1 || t > closestDistance) continue; // Behind camera or not closest
        
        // Calculate intersection point
        double hitX = rayOriginX + t * rayDirX;
        double hitY = rayOriginY + t * rayDirY;
        double hitZ = rayOriginZ + t * rayDirZ;
        
        // Check if intersection point is within wall bounds
        // Project hit point onto wall line
        double toHitX = hitX - x1;
        double toHitY = hitY - y1;
        double projection = (toHitX * wallDirX + toHitY * wallDirY);
        
        if (projection < 0 || projection > wallLength) continue; // Outside wall segment
        
        // Check if hit point is within wall height bounds
        double wallBase = wall->posZ / 16.0; // Wall base height
        if (hitZ < wallBase || hitZ > wallBase + height) continue; // Outside wall height
        
        // This is a valid hit and closer than previous hits
        closestDistance = t;
        foundHit = true;
        
        hitInfo.hit = true;
        hitInfo.hitX = hitX;
        hitInfo.hitY = hitY;
        hitInfo.hitZ = hitZ;
        hitInfo.normalX = wallNormalX;
        hitInfo.normalY = wallNormalY;
        hitInfo.normalZ = 0; // Walls are vertical, so normal has no Z component
        hitInfo.wallEntity = wall;
    }
    
    return foundHit;
}

void New3DRenderer::renderNormalVectorArrow(Map* map, RendTarget& target)
{
    if (!currentWallHit.hit) return;
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Use whiteTexture
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    
    // Convert world coordinates to camera-relative coordinates
    double camWorldX = m_camera.getX() / 16.0;
    double camWorldZ = m_camera.getY() / 16.0;
    
    double arrowX = currentWallHit.hitX - camWorldX;
    double arrowY = currentWallHit.hitY - camWorldZ;
    double arrowZ = currentWallHit.hitZ;
    
    // Arrow properties
    double arrowLength = 0.5; // Length of arrow in world units
    double arrowHeadSize = 0.1; // Size of arrow head
    double arrowLineWidth = 0.02; // Width of arrow line
    
    // Calculate arrow end point
    double endX = arrowX + currentWallHit.normalX * arrowLength;
    double endY = arrowY + currentWallHit.normalY * arrowLength;
    double endZ = arrowZ + currentWallHit.normalZ * arrowLength;
    
    // Draw arrow shaft as a thick line
    glColor4f(1.0f, 0.0f, 0.0f, 0.8f); // Red color with transparency
    glBegin(GL_QUADS);
    
    // Create perpendicular directions for thick line
    double perpX = -currentWallHit.normalY;
    double perpY = currentWallHit.normalX;
    
    // Quad vertices for arrow shaft
    glVertex3d(arrowX + perpX * arrowLineWidth, arrowZ - arrowLineWidth, arrowY + perpY * arrowLineWidth);
    glVertex3d(arrowX - perpX * arrowLineWidth, arrowZ - arrowLineWidth, arrowY - perpY * arrowLineWidth);
    glVertex3d(endX - perpX * arrowLineWidth, endZ - arrowLineWidth, endY - perpY * arrowLineWidth);
    glVertex3d(endX + perpX * arrowLineWidth, endZ - arrowLineWidth, endY + perpY * arrowLineWidth);
    
    glEnd();
    
    // Draw arrow head as a cone
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Solid red for arrow head
    glBegin(GL_TRIANGLES);
    
    // Create cone triangles around the arrow head
    int segments = 8;
    for (int i = 0; i < segments; ++i) {
        double angle1 = i * 2.0 * 3.14159 / segments;
        double angle2 = (i + 1) * 2.0 * 3.14159 / segments;
        
        double offsetX1 = currentWallHit.normalX * arrowHeadSize + perpX * std::cos(angle1) * arrowHeadSize * 0.5;
        double offsetY1 = currentWallHit.normalY * arrowHeadSize + perpY * std::cos(angle1) * arrowHeadSize * 0.5;
        double offsetZ1 = std::sin(angle1) * arrowHeadSize * 0.5;
        
        double offsetX2 = currentWallHit.normalX * arrowHeadSize + perpX * std::cos(angle2) * arrowHeadSize * 0.5;
        double offsetY2 = currentWallHit.normalY * arrowHeadSize + perpY * std::cos(angle2) * arrowHeadSize * 0.5;
        double offsetZ2 = std::sin(angle2) * arrowHeadSize * 0.5;
        
        // Triangle from arrow tip to base
        glVertex3d(endX, endZ, endY);
        glVertex3d(endX - offsetX1, endZ - offsetZ1, endY - offsetY1);
        glVertex3d(endX - offsetX2, endZ - offsetZ2, endY - offsetY2);
    }
    
    glEnd();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

}