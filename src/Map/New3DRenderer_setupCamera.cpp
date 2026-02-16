#include "Map/New3DRenderer.h"
#include "Game/Game.h"

namespace nyaa {

void New3DRenderer::setupCamera(RendTarget& target)
{
    m_camera.update(G->frameDeltaMillis / 1000.0f);
    m_camera.use(target);
}

}