#pragma once
#include "Game/Globals.h"
#include "Sound/SoundFxEngine.h"
#include <array>

namespace nyaa {

class Sfx
{
public:
    static SoundFxEngine Engine;

    static constexpr int MaxCoughChannels = 8;
    static std::array<SoundFxEngine::Fx*, MaxCoughChannels> ActiveCoughChannels;

	static SoundFxEngine::Fx* CursorMove;
    static SoundFxEngine::Fx* CursorSel;
    static SoundFxEngine::Fx* Shoot;
    static SoundFxEngine::Fx* WallHit;
    static SoundFxEngine::Fx* Hurt;
    static SoundFxEngine::Fx* MissionFailed;
    static SoundFxEngine::Fx* Wet;
    static SoundFxEngine::Fx* Close;
    static SoundFxEngine::Fx* Open;
    static SoundFxEngine::Fx* Pick;
	static SoundFxEngine::Fx* StoneLeftFoot;
    static SoundFxEngine::Fx* StoneRightFoot;
    static SoundFxEngine::Fx* GravelLeftFoot;
    static SoundFxEngine::Fx* GravelRightFoot;
	static SoundFxEngine::Fx* CarDoorOpen;
    static SoundFxEngine::Fx* CarDoorClose;
    static SoundFxEngine::Fx* CarGearShift;
    static SoundFxEngine::Fx* CarDrift;
    static SoundFxEngine::Fx* Cough1;
    static SoundFxEngine::Fx* Cough2;
    static SoundFxEngine::Fx* Cough3;
    static SoundFxEngine::Fx* Cough4;
    static SoundFxEngine::Fx* Cough5;
    static SoundFxEngine::Fx* Cough6;
    static SoundFxEngine::Fx* Cough7;
    static SoundFxEngine::Fx* Cough8;
    static SoundFxEngine::Fx* Cough9;
    static SoundFxEngine::Fx* Cough10;
    static SoundFxEngine::Fx* Cough11;
    static SoundFxEngine::Fx* Cough12;
    static SoundFxEngine::Fx* Cough13;
    static SoundFxEngine::Fx* Cough14;
    static SoundFxEngine::Fx* Cough15;
    static SoundFxEngine::Fx* Cough16;
    static SoundFxEngine::Fx* Cough17;
    static SoundFxEngine::Fx* Cough18;
    static SoundFxEngine::Fx* Cough19;

    static void PlayRandomCough(int channel);
};

}