#include "Game/FMRadio.h"
#include <miniaudio.h>
#include "Game/Game.h"
#include "Game/Util.h"
#include "Sound/SoundFxEngine.h"
#include "Game/Sfx.h"
#include "Screen/InGameScreen.h"
#include <algorithm>
#include <random>
#include <cassert>
#include "Entity/Actor.h"

namespace nyaa {

std::vector<const char*> RadioStation::Names =
{
    "ROCIAN 98.5",
    "Peam 99.7",
    "MeMum 101.4",
    "All the songz :3",
};

const char* RadioStation::GetStationName(Station stationType)
{
    return Names[stationType];
}

RadioStation::RadioStation(Station station)
{
    switch (station)
    {
    case StationCount:
    case Station_NULL:
    {
        //assert(false);
        return;
    }
    case Station_ROCIAN:
    {
        tracks.push_back("radio_if_u_aint_from_my_hood.mp3");
        tracks.push_back("radio_allwhitebuffies.mp3");
        break;
    }
    case Station_Peam:
    {
        tracks.push_back("radio_good_ass_day.mp3");
        tracks.push_back("radio_piss_wrist.mp3");
        break;
    }
    case Station_MeMum:
    {
        tracks.push_back("radio_lil_xan_leash.mp3");
        tracks.push_back("radio_raevloli_dont_kill_that_loli.mp3");
        // tracks.push_back("radio_me_mum_1.mp3");
        // tracks.push_back("radio_me_mum_2.mp3");
        // tracks.push_back("radio_me_mum_3.mp3");
        // tracks.push_back("radio_me_mum_4.mp3");
        // tracks.push_back("radio_me_mum_5.mp3");
        // tracks.push_back("radio_me_mum_6.mp3");
        break;
    }
    case Station_All:
    {
        tracks.push_back("radio_josip_all_i_do.mp3");
        tracks.push_back("radio_if_u_aint_from_my_hood.mp3");
        tracks.push_back("radio_me_mum_5.mp3");
        tracks.push_back("radio_allwhitebuffies.mp3");
        tracks.push_back("radio_me_mum_1.mp3");
        tracks.push_back("radio_josip_sushi.mp3");
        tracks.push_back("radio_me_mum_3.mp3");
        tracks.push_back("radio_good_ass_day.mp3");
        tracks.push_back("radio_crack_amico_return_of_the_crack.mp3");
        tracks.push_back("radio_me_mum_4.mp3");
        tracks.push_back("radio_josip_fuckthetrap.mp3");
        tracks.push_back("radio_me_mum_2.mp3");
        tracks.push_back("radio_josip_murakami.mp3");
        tracks.push_back("radio_piss_wrist.mp3");
        tracks.push_back("radio_me_mum_6.mp3");
        tracks.push_back("radio_josip_yoshi_island.mp3");
        tracks.push_back("radio_crack_amigo_predator.mp3");
        tracks.push_back("radio_scales_in_tha_kitchen.mp3");
        tracks.push_back("radio_knuckles_ball_till_we_fall.mp3");
        tracks.push_back("radio_knuckles_mercinary_killers.mp3");
        tracks.push_back("radio_knuckles_slanging_tapes.mp3");
        break;
    }
    }

    std::mt19937 rng{std::random_device{}()};
    std::shuffle(tracks.begin(), tracks.end(), rng);
}

RadioStation::~RadioStation()
{
}

FMRadio::FMRadio()
    :randomIndexPhase((int)(Util::RandNormalized() * 50))
    ,index(0)
    ,music(new ma_sound)
    ,musicNeedUninit(false)
    ,station(nullptr)
    ,stationType(RadioStation::Station_NULL)
{
}

void FMRadio::LoadStation(RadioStation::Station stationType, bool autoplay)
{
    if(stationType == RadioStation::Station_NULL) return;
    Stop();
    if (station) delete station;
    station = new RadioStation(stationType);
    this->stationType = stationType;
    if(autoplay) PickRandomSong();
    
    // Show station name when switching stations
    if (IGS && IGS->player && IGS->player->carImInsideOf) {
        IGS->showRadioStation(RadioStation::GetStationName(stationType));
    }
}

FMRadio::~FMRadio()
{
    if (musicNeedUninit) ma_sound_uninit(music);
    delete station;
	delete music;
}

//void FMRadio::ScanRes()
//{
//    SongPaths.clear();
//    for (const auto& entry : std::filesystem::directory_iterator(Game::ResDir))
//    {
//        std::string filename = entry.path().filename().string();
//        if (filename.length() >= 6 && (filename.substr(0, 6) == "radio_"))
//        {
//            SongPaths.push_back(entry.path().string());
//        }
//    }
//}

void FMRadio::PickRandomSong(bool randSeek)
{
    if (station->tracks.empty()) return;
	auto ts = station->tracks.size();
    index = ((index + 1) + randomIndexPhase) % ts;
    auto& pick = station->tracks[/* Util::Clamp( */index/* , 0, ts - 1) */];

    ma_engine* engine = Sfx::Engine.engine;
    if(musicNeedUninit) ma_sound_uninit(music);
    auto result = ma_sound_init_from_file(engine, (std::string(Game::ResDir) + std::string(pick)).c_str(), 0, NULL, NULL, music);
    if (result == MA_SUCCESS)
    {
        // Attach to FLC node to route through node graph
        ma_node_attach_output_bus((ma_node*)music, 0, (ma_node*)Sfx::Engine.flcNode, 0);
        ma_sound_set_positioning(music, ma_positioning_relative);
        ma_sound_set_position(music, 0.0f, 0.0f, 0.0f);
        ma_sound_set_volume(music, Sfx::Engine.radioVolume);
        ma_uint64 lengthFrames = 0;
        result = ma_sound_get_length_in_pcm_frames(music, &lengthFrames);
        if (result == MA_SUCCESS)
        {
            //ma_sound_set_looping(&music, true);
            if(randSeek) 
                ma_sound_seek_to_pcm_frame(music, (ma_uint64)((float)lengthFrames * Util::RandNormalized()) % lengthFrames);
            result = ma_sound_start(music);
            if (result == MA_SUCCESS)
            {
                //Util::PrintLnFormat("");
            }
        }
        musicNeedUninit = true;
    }
}

void FMRadio::Tick()
{
    if (!ma_sound_is_playing(music))
    {
        PickRandomSong(false);
    }
}

void FMRadio::Stop()
{
    ma_sound_stop(music);
}

void FMRadio::PrevStation()
{
    auto nst = (RadioStation::Station)((stationType - 1) % RadioStation::StationCount);
    LoadStation(nst, true);
}

void FMRadio::NextStation()
{
    auto nst = (RadioStation::Station)((stationType + 1) % RadioStation::StationCount);
    LoadStation(nst, true);
}

void FMRadio::Pause()
{
    if (musicNeedUninit && ma_sound_is_playing(music))
    {
        ma_sound_stop(music);
    }
}

void FMRadio::Resume()
{
    if (musicNeedUninit && !ma_sound_is_playing(music))
    {
        ma_sound_start(music);
    }
}

void FMRadio::updateVolume()
{
    if (musicNeedUninit)
    {
        ma_sound_set_volume(music, Sfx::Engine.radioVolume);
    }
}

}