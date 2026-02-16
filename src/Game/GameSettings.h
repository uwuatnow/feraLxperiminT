#pragma once
#include "Game/Globals.h"
#include <string>
#include "Serialize/Serializable.h"

namespace nyaa {

class GameSettings : public Serializable
{
public:
    GameSettings();

    ~GameSettings();

public:
    void save();
    
    void load();

public:
    float masterVolume;
    float radioVolume;
    bool uncapFramerate;

    static GameSettings* GlobalSettings;
};

} // namespace nyaa