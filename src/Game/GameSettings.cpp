#include "Game/GameSettings.h"
#include "Game/Util.h"
#include "Sound/SoundFxEngine.h"
#include "Game/Sfx.h"

namespace nyaa {

GameSettings* GameSettings::GlobalSettings = nullptr;

GameSettings::GameSettings()
    :Serializable("GameSettings", nullptr, "./settings.json")
    ,masterVolume(1.0f)
    ,radioVolume(0.6f)
    ,uncapFramerate(false)
{
    NYAA_SERIAL_ADD(this, masterVolume,);
    NYAA_SERIAL_ADD(this, radioVolume,);
    NYAA_SERIAL_ADD(this, uncapFramerate,);

    load();
}

GameSettings::~GameSettings()
{
    save();
}

void GameSettings::save()
{
    masterVolume = Sfx::Engine.masterVolume;
    radioVolume = Sfx::Engine.radioVolume;
    upload();
}

void GameSettings::load()
{
    Read();
    Sfx::Engine.masterVolume = masterVolume;
    Sfx::Engine.radioVolume = radioVolume;
}

} // namespace nyaa