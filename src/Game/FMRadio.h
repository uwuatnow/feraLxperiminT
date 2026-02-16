#pragma once
#include "Game/Globals.h"
#include <vector>

struct ma_sound;

namespace nyaa {

class RadioStation
{
	static std::vector<const char*> Names;

public:
	std::vector<const char*> tracks;
	
	enum Station : int
	{
		Station_NULL = -1,

		Station_ROCIAN,
		Station_Peam,
		Station_MeMum,
        Station_All,

		StationCount
	};

	static const char* GetStationName(Station stationType);

	RadioStation(Station station);

	~RadioStation();
};

class FMRadio
{
    int randomIndexPhase;
    int index;

    ::ma_sound* music;
    bool musicNeedUninit;

    RadioStation* station;
    RadioStation::Station stationType;

public:
    void LoadStation(RadioStation::Station stationType, bool autoplay);

    FMRadio();

    ~FMRadio();

    //populate available radio songs from res folder
    //static void ScanRes();


    void PickRandomSong(bool randSeek = true);

    //call this every frame and radio is active
    void Tick();

    //stop playback
    void Stop();

    //pause playback
    void Pause();

    //resume playback
    void Resume();

    void PrevStation();

    void NextStation();

    void updateVolume();

    RadioStation::Station getCurrentStation() const { return stationType; }
};
}