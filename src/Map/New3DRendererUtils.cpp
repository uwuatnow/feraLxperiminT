#include "Map/New3DRenderer.h"

namespace nyaa {

void New3DRenderer::screenToWorld(int screenPosX, int screenPosY, RendTarget& target, double* outX, double* outY) {
    m_camera.screenToWorld(screenPosX, screenPosY, target, outX, outY);
}

void New3DRenderer::worldToScreen(double worldPosX, double worldPosY, RendTarget& target, int* outX, int* outY) {
    m_camera.worldToScreen(worldPosX, worldPosY, target, outX, outY);
}

}