#include "Game/Sfx.h"
#include "Game/Util.h"

namespace nyaa {

SoundFxEngine Sfx::Engine;
std::array<SoundFxEngine::Fx*, Sfx::MaxCoughChannels> Sfx::ActiveCoughChannels;

SoundFxEngine::Fx* Sfx::CursorMove;
SoundFxEngine::Fx* Sfx::CursorSel;
SoundFxEngine::Fx* Sfx::Shoot;
SoundFxEngine::Fx* Sfx::WallHit;
SoundFxEngine::Fx* Sfx::Hurt;
SoundFxEngine::Fx* Sfx::MissionFailed;
SoundFxEngine::Fx* Sfx::Wet;
SoundFxEngine::Fx* Sfx::Close;
SoundFxEngine::Fx* Sfx::Open;
SoundFxEngine::Fx* Sfx::Pick;
SoundFxEngine::Fx* Sfx::StoneLeftFoot;
SoundFxEngine::Fx* Sfx::StoneRightFoot;
SoundFxEngine::Fx* Sfx::GravelLeftFoot;
SoundFxEngine::Fx* Sfx::GravelRightFoot;
SoundFxEngine::Fx* Sfx::CarDoorOpen;
SoundFxEngine::Fx* Sfx::CarDoorClose;
SoundFxEngine::Fx* Sfx::CarGearShift;
SoundFxEngine::Fx* Sfx::CarDrift;
SoundFxEngine::Fx* Sfx::GunRattle1;
SoundFxEngine::Fx* Sfx::GunRattle2;
SoundFxEngine::Fx* Sfx::GunRattle3;
SoundFxEngine::Fx* Sfx::GunRattle4;
SoundFxEngine::Fx* Sfx::Cough1;
SoundFxEngine::Fx* Sfx::Cough2;
SoundFxEngine::Fx* Sfx::Cough3;
SoundFxEngine::Fx* Sfx::Cough4;
SoundFxEngine::Fx* Sfx::Cough5;
SoundFxEngine::Fx* Sfx::Cough6;
SoundFxEngine::Fx* Sfx::Cough7;
SoundFxEngine::Fx* Sfx::Cough8;
SoundFxEngine::Fx* Sfx::Cough9;
SoundFxEngine::Fx* Sfx::Cough10;
SoundFxEngine::Fx* Sfx::Cough11;
SoundFxEngine::Fx* Sfx::Cough12;
SoundFxEngine::Fx* Sfx::Cough13;
SoundFxEngine::Fx* Sfx::Cough14;
SoundFxEngine::Fx* Sfx::Cough15;
SoundFxEngine::Fx* Sfx::Cough16;
SoundFxEngine::Fx* Sfx::Cough17;
SoundFxEngine::Fx* Sfx::Cough18;
SoundFxEngine::Fx* Sfx::Cough19;

void Sfx::PlayRandomCough(int channel)
{
    if (channel < 0 || channel >= MaxCoughChannels) return;

    // Stop any currently playing cough on this channel
    if (ActiveCoughChannels[channel] && ActiveCoughChannels[channel]->isPlaying())
    {
        ActiveCoughChannels[channel]->stop();
    }

    // Select random cough (1-19)
    int coughIdx = 1 + (Util::RandNormalized() * 19);
    if (coughIdx < 1) coughIdx = 1;
    if (coughIdx > 19) coughIdx = 19;

    SoundFxEngine::Fx* coughFx = nullptr;
    switch (coughIdx)
    {
        case 1: coughFx = Cough1; break;
        case 2: coughFx = Cough2; break;
        case 3: coughFx = Cough3; break;
        case 4: coughFx = Cough4; break;
        case 5: coughFx = Cough5; break;
        case 6: coughFx = Cough6; break;
        case 7: coughFx = Cough7; break;
        case 8: coughFx = Cough8; break;
        case 9: coughFx = Cough9; break;
        case 10: coughFx = Cough10; break;
        case 11: coughFx = Cough11; break;
        case 12: coughFx = Cough12; break;
        case 13: coughFx = Cough13; break;
        case 14: coughFx = Cough14; break;
        case 15: coughFx = Cough15; break;
        case 16: coughFx = Cough16; break;
        case 17: coughFx = Cough17; break;
        case 18: coughFx = Cough18; break;
        case 19: coughFx = Cough19; break;
    }

    if (coughFx)
    {
        ActiveCoughChannels[channel] = coughFx;
        coughFx->play();
    }
}

void Sfx::PlayRandomGunRattle(float x, float y)
{
    SoundFxEngine::Fx* rattles[4] = { GunRattle1, GunRattle2, GunRattle3, GunRattle4 };
    int idx = (int)(Util::RandNormalized() * 4.0f);
    if (idx < 0) idx = 0;
    if (idx > 3) idx = 3;
    if (rattles[idx])
        rattles[idx]->play(x, y);
}

}