#include "Game/Camera.h"
#include "Screen/InGameScreen.h"
#include "Game/Game.h"
#include "Entity/Actor.h"
#include "Game/Mouse.h"
#include <SFML/OpenGL.hpp>
#include <GL/glu.h>
#include <cmath>
#include <algorithm>
#include "Map/Pathfinding.h"
#include "Entity/DevCar.h"
#include "Game/Util.h"
#include "Game/Sfx.h"
#include "Game/Kb.h"
#include "Game/Controller.h"
#include <SFML/Window/Joystick.hpp>

namespace nyaa {

Camera::Camera()
    :posX(0.0), posY(0.0)
    ,targetCameraDistance(25.0)
    ,currentCameraDistance(25.0)
    ,cameraHeight(25.0)
    ,lastCamZoom(1.0f)
    ,camZoom(1.0f)
    ,zoomFactor(0.0)
    ,lookAtY(2.0f)
    ,cameraTargetPosX(0.0), cameraTargetPosY(0.0)
    ,cameraVelocityX(0.0), cameraVelocityY(0.0)
    ,cameraFollowingPath(false)
    ,carCameraSmoothX(0.0), carCameraSmoothY(0.0)
    ,carCameraSmoothVelX(0.0), carCameraSmoothVelY(0.0)
    ,carCameraHeightSmooth(0.0), carCameraHeightVel(0.0)
    ,carCameraDistSmooth(0.0), carCameraDistVel(0.0)
    ,carLookAtSmoothX(0.0), carLookAtSmoothY(0.0)
    ,carLookAtSmoothVelX(0.0), carLookAtSmoothVelY(0.0)
    ,lastPlayerInCar(false)
    ,offsetX(0.0), offsetY(0.0)
    ,dragStartMousePosX(0), dragStartMousePosY(0)
    ,lerpSpeed(0.15)
    ,basisValid(false)
    ,horizontalLookAtOffset(0.0)
{
}

Camera::~Camera()
{
}

void Camera::update(float deltaTime)
{
    if (!IGS || !IGS->player) return;

    // Handle zoom factor
    zoomFactor = (IGS->camZoom - 1.0f) / 9.0f; // Normalize zoom to 0-1 range (camZoom 1-10)
    zoomFactor = std::max(0.0f, std::min(zoomFactor, 1.0f));

    bool playerInCar = (IGS->player->carImInsideOf != nullptr);
    CarBase* car = IGS->player->carImInsideOf;

    if (IGS->initialCam) {
        double effectivePlayerX = IGS->player->posX;
        double effectivePlayerY = IGS->player->posY;
        
        if (playerInCar) {
            effectivePlayerX = car->posX;
            effectivePlayerY = car->posY;
            
            carCameraSmoothX = effectivePlayerX;
            carCameraSmoothY = effectivePlayerY;
            carCameraSmoothVelX = 0.0f;
            carCameraSmoothVelY = 0.0f;
            
            // Also reset look-at smoothing for car
            carLookAtSmoothX = effectivePlayerX / 16.0f;
            carLookAtSmoothY = effectivePlayerY / 16.0f + 0.5f;
            carLookAtSmoothVelX = 0.0f;
            carLookAtSmoothVelY = 0.0f;
        }

        posX = effectivePlayerX;
        posY = effectivePlayerY;
        cameraTargetPosX = effectivePlayerX;
        cameraTargetPosY = effectivePlayerY; // Fix type conversion warning
        cameraVelocityX = 0.0f;
        cameraVelocityY = 0.0f;
    }

    float targetDistance, targetHeight;

    if (IGS->showInventoryPopup || IGS->dbox.showing) {
        targetDistance = 8.0f;
        targetHeight = 3.5f;

        currentCameraDistance += (targetDistance - currentCameraDistance) * lerpSpeed;
        cameraHeight += (targetHeight - cameraHeight) * lerpSpeed;
    } else if (playerInCar) {
        float carSpeed = std::sqrt(car->velX * car->velX + car->velY * car->velY);
        float speedFactor = std::min(carSpeed / 200.0f, 1.0f);
        
        float baseDistance = 18.0f;
        float baseHeight = 15.0f;
        
        targetDistance = baseDistance - zoomFactor * 12.0f + (speedFactor * 4.0f);
        targetHeight = baseHeight - zoomFactor * 12.0f - (speedFactor * 3.0f);
        
        float velocityDirRad = std::atan2(car->velY, car->velX);
        float dirDiff = std::abs(Util::AngleDifference(car->dirAngle, Util::ToDeg(velocityDirRad)));
        bool isReversing = (dirDiff > 90.0f);
        
        if (isReversing) {
            targetDistance += 6.0f;
            targetHeight += 4.0f;
        }

        // Smooth car-specific damping - use double precision for stability at large distances
        double carDamping = 0.92;
        double desiredDistVel = (targetDistance - carCameraDistSmooth) * 8.0;
        carCameraDistVel = (carCameraDistVel + desiredDistVel * (double)deltaTime) * carDamping;
        carCameraDistSmooth += carCameraDistVel * (double)deltaTime;
        currentCameraDistance = carCameraDistSmooth;
        
        double desiredHeightVel = (targetHeight - carCameraHeightSmooth) * 8.0;
        carCameraHeightVel = (carCameraHeightVel + desiredHeightVel * (double)deltaTime) * carDamping;
        carCameraHeightSmooth += carCameraHeightVel * (double)deltaTime;
        cameraHeight = carCameraHeightSmooth;
    } else {
        targetDistance = 25.0f - zoomFactor * 22.0f;
        targetHeight = 25.0f - zoomFactor * 22.0f;
        
        currentCameraDistance += (targetDistance - currentCameraDistance) * (double)lerpSpeed;
        cameraHeight += (targetHeight - cameraHeight) * (double)lerpSpeed;
    }

    // Update horizontal look-at offset for inventory view - use double precision for stability
    double targetHorizontalLookAtOffset = (IGS->showInventoryPopup || IGS->dbox.showing) ? -1.3 : 0.0;
    horizontalLookAtOffset += (targetHorizontalLookAtOffset - horizontalLookAtOffset) * (double)lerpSpeed;

    // Follow logic
    double effectivePlayerX = IGS->player->posX;
    double effectivePlayerY = IGS->player->posY;
    if (playerInCar) {
        effectivePlayerX = car->posX;
        effectivePlayerY = car->posY;
    }

    if (posX == 0 && posY == 0) {
        posX = effectivePlayerX;
        posY = effectivePlayerY;
        cameraTargetPosX = effectivePlayerX;
        cameraTargetPosY = effectivePlayerY;
        lastCamZoom = IGS->camZoom;
    }

    if (playerInCar != lastPlayerInCar) {
        if (playerInCar) {
            carCameraSmoothX = posX;
            carCameraSmoothY = posY;
            carCameraSmoothVelX = 0.0f;
            carCameraSmoothVelY = 0.0f;
            carCameraHeightSmooth = cameraHeight;
            carCameraHeightVel = 0.0f;
            carCameraDistSmooth = currentCameraDistance;
            carCameraDistVel = 0.0f;
            carLookAtSmoothX = posX / 16.0f;
            carLookAtSmoothY = posY / 16.0f + 0.5f;
            carLookAtSmoothVelX = 0.0f;
            carLookAtSmoothVelY = 0.0f;
        } else {
            posX = carCameraSmoothX;
            posY = carCameraSmoothY;
            cameraHeight = carCameraHeightSmooth;
            currentCameraDistance = carCameraDistSmooth;
        }
        lastPlayerInCar = playerInCar;
    }

    if (lastCamZoom != IGS->camZoom) {
        lastCamZoom = IGS->camZoom;
    }

    handleMiddleClickDrag();

    if (cameraFollowingPath && IGS->player->astn && IGS->player->ast && IGS->player->ast->valid()) {
        PathfindNode* nextNode = IGS->player->astn;
        if (nextNode && nextNode->child) {
            cameraTargetPosX = nextNode->child->posX;
            cameraTargetPosY = nextNode->child->posY;
        } else {
            cameraTargetPosX = nextNode->posX;
            cameraTargetPosY = nextNode->posY;
        }
        
        double dx = cameraTargetPosX - posX;
        double dy = cameraTargetPosY - posY;
        cameraVelocityX += dx * 0.05;
        cameraVelocityY += dy * 0.05;
        cameraVelocityX *= 0.85;
        cameraVelocityY *= 0.85;
        
        posX += cameraVelocityX * (double)deltaTime * 10.0;
        posY += cameraVelocityY * (double)deltaTime * 10.0;
    } else {
        float moveDist = 10.0f;
        float distFromCam = std::sqrt((effectivePlayerX - posX) * (effectivePlayerX - posX) +
                                     (effectivePlayerY - posY) * (effectivePlayerY - posY));
        
        if (distFromCam > moveDist) {
            float xdist = effectivePlayerX - posX;
            float ydist = effectivePlayerY - posY;
            float spd = IGS->player->moveAmountMax * deltaTime * 60.0f;
            if(!IGS->plrOnScreen) spd *= 3.33f;
            
            posX += Util::Clamp(xdist, -spd, spd);
            posY += Util::Clamp(ydist, -spd, spd);
        }
    }

    if (playerInCar) {
        float carSpeed = std::sqrt(car->velX * car->velX + car->velY * car->velY);
        float speedFactor = std::min(carSpeed / 150.0f, 1.0f);
        double leadDist = (10.0 * 16.0) * speedFactor; // Lead ahead by up to 10 tiles

        double targetPosX = car->posX;
        double targetPosY = car->posY;

        float vX = car->velX, vY = car->velY;
        float vLen = std::sqrt(vX*vX + vY*vY);
        if (vLen > 0.1f) {
            targetPosX += (double)(vX / vLen) * leadDist;
            targetPosY += (double)(vY / vLen) * leadDist;
        } else {
            // If nearly stationary, subtly lead in car's facing direction
            float carDirRad = Util::ToRad(car->dirAngle);
            targetPosX += (double)std::cos(carDirRad) * (2.0 * 16.0) * speedFactor;
            targetPosY += (double)std::sin(carDirRad) * (2.0 * 16.0) * speedFactor;
        }

        double damping = 0.94;
        double desiredVelX = (targetPosX - carCameraSmoothX) * 10.0;
        double desiredVelY = (targetPosY - carCameraSmoothY) * 10.0;
        
        carCameraSmoothVelX = (carCameraSmoothVelX + desiredVelX * (double)deltaTime) * damping;
        carCameraSmoothVelY = (carCameraSmoothVelY + desiredVelY * (double)deltaTime) * damping;
        carCameraSmoothX += carCameraSmoothVelX * (double)deltaTime;
        carCameraSmoothY += carCameraSmoothVelY * (double)deltaTime;
        
        posX = carCameraSmoothX;
        posY = carCameraSmoothY;
    }

    basisValid = false;
}

void Camera::handleMiddleClickDrag()
{
    if (Mouse::MiddleFrames == 1) {
        dragStartMousePosX = Mouse::Pos_X;
        dragStartMousePosY = Mouse::Pos_Y;
        offsetX = offsetY = 0;
    }
    
    if (Mouse::MiddleFrames > 0) {
        sf::Vector2i mouseDelta = sf::Vector2i{ Mouse::Pos_X, Mouse::Pos_Y } - sf::Vector2i{ dragStartMousePosX, dragStartMousePosY };
        double scale = (25.0 - zoomFactor * 22.0) * 0.01;
        offsetX = -mouseDelta.x * scale;
        offsetY = -mouseDelta.y * scale;
        
        double followDist = 48.0;
        double offsetDist = sqrt(offsetX * offsetX + offsetY * offsetY);
        if (offsetDist > followDist) {
            double m = followDist / offsetDist;
            offsetX *= m;
            offsetY *= m;
        }
    } else {
        offsetX *= 0.9;
        offsetY *= 0.9;
        if (abs(offsetX) < 0.1 && abs(offsetY) < 0.1) offsetX = offsetY = 0;
    }
}

void Camera::use(sf::RenderTarget& target, float verticalOffset)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Use double precision for projection matrix setup to maintain accuracy at large distances
    double aspectRatio = (double)target.getSize().x / (double)target.getSize().y;
    gluPerspective(45.0, (float)aspectRatio, 1.0, 1000.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Point finalPos = { posX + (double)offsetX, posY + (double)offsetY };
    // Use double precision for camera position calculations to maintain accuracy at large distances
    double cX = finalPos.x / 16.0;
    double cZ = finalPos.y / 16.0;

    bool playerInCar = (IGS && IGS->player && IGS->player->carImInsideOf != nullptr);
    CarBase* car = playerInCar ? IGS->player->carImInsideOf : nullptr;

    lookAtY = 2.0f - zoomFactor * 2.0f;

    if (playerInCar) {
        float carSpeed = std::sqrt(car->velX * car->velX + car->velY * car->velY);
        float speedFactor = std::min(carSpeed / 200.0f, 1.0f);
        float reversingFactor = (carSpeed > 10.0f && std::abs(Util::AngleDifference(car->dirAngle, Util::ToDeg(std::atan2(car->velY, car->velX)))) > 90.0f) ? 0.5f : 0.0f;
        
        // Slightly shift the vertical focus at speed to see more of the distance
        lookAtY = (2.0f - zoomFactor * 2.0f) - (speedFactor * 1.5f) + reversingFactor;
    }

    // We use Camera-Relative Rendering to avoid precision loss at large world coordinates.
    // The GPU origin (0,0,0) is set to the camera's world position.
    double eyeX = 0;
    double eyeY = cameraHeight + verticalOffset;
    double eyeZ = currentCameraDistance;
    
    double centerX = horizontalLookAtOffset;
    double centerY = lookAtY + verticalOffset;
    double centerZ = 0;

    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, 0, 1, 0);
    
    // Cache absolute positions for basis/audio which still need them
    eyePos.x = cX;
    eyePos.y = eyeY;
    eyePos.z = cZ + currentCameraDistance;
    laX = cX + horizontalLookAtOffset;
    laY = centerY;
    laZ = cZ;

    updateBasis(target);

    // Update audio listener
    float ldirX = laX - eyePos.x;
    float ldirY = laY - eyePos.y;
    float ldirZ = laZ - eyePos.z;
    Sfx::Engine.setListener((float)(cX * 16.0f), (cameraHeight + verticalOffset) * 16.0f, (float)((cZ + currentCameraDistance) * 16.0f), ldirX, ldirY, ldirZ, 0, 1, 0);
}

void Camera::updateBasis(const sf::RenderTarget& target) const
{
    if (basisValid) return;

    // Use double precision for basis calculations to maintain accuracy at large distances
    double dx = laX - eyePos.x;
    double dy = laY - eyePos.y;
    double dz = laZ - eyePos.z;
    double len = sqrt(dx*dx + dy*dy + dz*dz);
    if (len < 0.0001) len = 1.0;
    forward = {dx/len, dy/len, dz/len};
    
    right.x = -forward.z;
    right.y = 0;
    right.z = forward.x;
    double slen = sqrt(right.x*right.x + right.z*right.z);
    if (slen < 0.0001) { right.x = 1.0; right.z = 0.0; } else {
        right.x = right.x / slen;
        right.z = right.z / slen;
    }
    
    up.x = -right.z*forward.y;
    up.y = right.z*forward.x - right.x*forward.z;
    up.z = right.x*forward.y;

    basisValid = true;
}

void Camera::screenToWorld(int screenX, int screenY, sf::RenderTarget& target, double* outX, double* outY) const
{
    updateBasis(target);

    double screenWidth = (double)target.getSize().x;
    double screenHeight = (double)target.getSize().y;
    double ndcX = (2.0 * screenX) / screenWidth - 1.0;
    double ndcY = 1.0 - (2.0 * screenY) / screenHeight;
    
    double fov = 45.0 * 3.141592653589793 / 180.0;
    double aspect = screenWidth / screenHeight;
    double tanHalfFov = tan(fov * 0.5);
    
    double rayDirCamX = ndcX * tanHalfFov * aspect;
    double rayDirCamY = ndcY * tanHalfFov;
    
    double wDirX = rayDirCamX * right.x + rayDirCamY * up.x + forward.x;
    double wDirY = rayDirCamX * right.y + rayDirCamY * up.y + forward.y;
    double wDirZ = rayDirCamX * right.z + rayDirCamY * up.z + forward.z;
    
    if (std::abs(wDirY) > 0.0001) {
        double t = -eyePos.y / wDirY;
        if (t > 0.0) {
            if (outX) *outX = (eyePos.x * 16.0) + (wDirX * t * 16.0);
            if (outY) *outY = (eyePos.z * 16.0) + (wDirZ * t * 16.0);
            return;
        }
    }
    
    if (outX) *outX = laX * 16.0;
    if (outY) *outY = laZ * 16.0;
}

void Camera::worldToScreen(double worldX, double worldY, sf::RenderTarget& target, int* outX, int* outY) const
{
    updateBasis(target);

    double screenWidth = (double)target.getSize().x;
    double screenHeight = (double)target.getSize().y;
    double wX = worldX / 16.0;
    double wZ = worldY / 16.0;
    
    double relX = wX - eyePos.x;
    double relY = -eyePos.y;
    double relZ = wZ - eyePos.z;
    
    double camSpaceX = right.x*relX + right.y*relY + right.z*relZ;
    double camSpaceY = up.x*relX + up.y*relY + up.z*relZ;
    double camSpaceZ = -forward.x*relX - forward.y*relY - forward.z*relZ;
    
    if (camSpaceZ < 0.0) {
        double aspect = screenWidth / screenHeight;
        double tanHalfFov = tan(45.0 * 3.141592653589793 / 180.0 * 0.5);
        
        double projX = camSpaceX / (-camSpaceZ * tanHalfFov * aspect);
        double projY = camSpaceY / (-camSpaceZ * tanHalfFov);
        
        if (outX) *outX = (int)((projX + 1.0) * screenWidth * 0.5);
        if (outY) *outY = (int)((1.0 - projY) * screenHeight * 0.5);
        return;
    }
    
    if (outX) *outX = -1;
    if (outY) *outY = -1;
}

}