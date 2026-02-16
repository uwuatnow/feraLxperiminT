#pragma once
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

namespace nyaa {

class Camera
{
public:
    Camera();
    virtual ~Camera();

    // Core logic
    void update(float deltaTime);
    
    // Setup projection and modelview
    void use(sf::RenderTarget& target, float verticalOffset = 0.0f);

    // Coordinate conversions
    void screenToWorld(int screenX, int screenY, sf::RenderTarget& target, double* outX, double* outY) const;
    void worldToScreen(double worldX, double worldY, sf::RenderTarget& target, int* outX, int* outY) const;

    // Direct access to state
    double getX() const { return posX + (double)offsetX; }
    double getY() const { return posY + (double)offsetY; }
    void setPosition(double x, double y) { posX = x; posY = y; }
    
    float getZoom() const { return camZoom; }
    void setZoom(float zoom) { camZoom = zoom; }

    float getOffsetX() const { return offsetX; }
    float getOffsetY() const { return offsetY; }
    
    float getHeight() const { return cameraHeight; }
    float getDistance() const { return currentCameraDistance; }
    float getLookAtY() const { return lookAtY; }

    // Car mode state
    double getCarLookAtX() const { return carLookAtSmoothX; }
    double getCarLookAtY() const { return carLookAtSmoothY; }
    
    // Basis vectors (cached during update/use)
    const sf::Vector3<double>& getEyePos() const { return eyePos; }
    const sf::Vector3<double>& getForward() const { return forward; }
    const sf::Vector3<double>& getRight() const { return right; }
    const sf::Vector3<double>& getUp() const { return up; }

    // Following path state
    void setFollowingPath(bool following) { cameraFollowingPath = following; }
    bool isFollowingPath() const { return cameraFollowingPath; }

    void handleMiddleClickDrag();

private:
    // Core position on map (2D)
    double posX, posY;
    
    // Viewing parameters
    float targetCameraDistance;
    float currentCameraDistance;
    float cameraHeight;
    float lastCamZoom;
    float camZoom;
    float zoomFactor;
    float lookAtY;

    // Follow properties
    double cameraTargetPosX, cameraTargetPosY;
    double cameraVelocityX, cameraVelocityY;
    bool cameraFollowingPath;

    // Car smoothing state
    double carCameraSmoothX, carCameraSmoothY;
    double carCameraSmoothVelX, carCameraSmoothVelY;
    double carCameraHeightSmooth, carCameraHeightVel;
    double carCameraDistSmooth, carCameraDistVel;
    double carLookAtSmoothX, carLookAtSmoothY;
    double carLookAtSmoothVelX, carLookAtSmoothVelY;
    bool lastPlayerInCar;

    // Dragging state
    float offsetX, offsetY;
    int dragStartMousePosX, dragStartMousePosY;

    // Interpolation helpers
    float lerpSpeed;

    // Cached basis vectors for conversions
    mutable sf::Vector3<double> eyePos, forward, right, up;
    mutable double laX, laY, laZ;
    mutable bool basisValid;

    float horizontalLookAtOffset;
    void updateBasis(const sf::RenderTarget& target) const;
};

}