#include "Screen/InGameScreen.h"
#include "Entity/Interaction.h"
#include "Screen/MapSwitchScreen.h"
#include "Screen/NewGameScreen.h"
#include "Map/MapFactory.h"
#include "Entity/Actor.h"
#include "Mission/Mission.h"
#include "Game/Game.h"
#include "Game/FMRadio.h"

namespace nyaa {

void InGameScreen::loadChunksNearPlayer()
{
    auto ca = player;
    ca->updateTPos();
    auto cm = mapFactory->getCurrentMap();
    int range = 1;
    for (int cy = ca->chunkPosY - range; cy < ca->chunkPosY + range + 1; cy++)
    {
        for (int cx = ca->chunkPosX - range; cx < ca->chunkPosX + range + 1; cx++)
        {
            cm->getChunk(cx, cy, true);
        }
    }
}

bool InGameScreen::doesIntersectView(float rectLeft, float rectTop, float rectWidth, float rectHeight)
{
    auto vr = sf::FloatRect{};
    getViewRect(&vr.left, &vr.top, &vr.width, &vr.height);
    return vr.intersects(sf::FloatRect{ rectLeft, rectTop, rectWidth, rectHeight });
}

bool InGameScreen::isPointInView(float pointX, float pointY)
{
    auto vr = sf::FloatRect{};
    getViewRect(&vr.left, &vr.top, &vr.width, &vr.height);
    return vr.contains(pointX, pointY);
}

void InGameScreen::getViewRect(float* outLeft, float* outTop, float* outWidth, float* outHeight)
{
    auto ret = sf::FloatRect(worldView.getCenter() - worldView.getSize() / 2.f, worldView.getSize());
    if(outLeft) *outLeft = ret.left;
    if(outTop) *outTop = ret.top;
    if(outWidth) *outWidth = ret.width;
    if(outHeight) *outHeight = ret.height;
}

void InGameScreen::mapPosToScreenPos(float mapPosX, float mapPosY, int* outX, int* outY)
{
    auto crds = rendTex.mapCoordsToPixel({ mapPosX, mapPosY });
    if (outX) *outX = crds.x;
    if (outY) *outY = crds.y;
}

void InGameScreen::addInteractablePriority(Interaction *inter)
{
    IGS->interactables.insert(IGS->interactables.begin(), inter);
    if(IGS->player->flags & Entity::EntFlag_Animating)
    {
        IGS->interactableIndex = 0;
    }
}

void InGameScreen::switchMaps(std::string toMapName)
{
    // Reset camera following state when switching maps
    cameraFollowingPath = false;
    playerAutoWalking = false;
    cameraPathPoints.clear();
    cameraVelocityX = 0.0f;
    cameraVelocityY = 0.0f;

    switchToMapName = toMapName;
    MapSwitchScreen::Instance->switchTo();
}

bool InGameScreen::onEnter()
{
    if (Screen::CurrentScreen == (Screen*)NewGameScreen::Instance)
    {
        initialCam = true;
        curMission->camSettled = false;
        loadChunksNearPlayer();
        G->screenSwitchClock.restart();
    }
    
    // Resume radio when entering game screen, but only if player is in a car
    if (fmRadio && player->carImInsideOf)
    {
        fmRadio->Resume();
    }
    
    return true;
}

bool InGameScreen::onLeave()
{
    // Pause radio when leaving game screen
    if (fmRadio)
    {
        fmRadio->Pause();
    }
    
    return true;
}

std::vector<Entity*> InGameScreen::getAllMissionEnts()
{
    std::vector<Entity*> ret;
    for (auto m : mapFactory->maps)
    {
        auto ev = m->getMissionSpawnedEnts();
        for (auto e : ev)
        {
            ret.push_back(e);
        }
    }
    return ret;
}

void InGameScreen::setMission(Mission* m)
{
    auto ame = IGS->getAllMissionEnts();
    for (auto e : ame)
    {
        e->flags |= Entity::EntFlag_Dead;
    }
    if(curMission)
    {
        delete curMission;
    }
    curMission = m;
}

} // namespace nyaa
