#pragma once
#include "Game/Globals.h"

namespace nyaa {

class DrugEffectQuantum
{
    //ALL OF THESE ARE 0.0 - 1.0 NORMALIZED RANGE.

public:
    DrugEffectQuantum();

public:
    float durationSeconds;

public: //player effects
    float playerWalkSpeedIncrease;
    float playerDiureticEffect;
    
public: //visual effects
	float visualWavy;
	float visualWorldVibration;
	float visualColorShift;
	float visualKeleidoscope;
	float visualNoise;
	float visualGreyscale;
	float visualShadowPeople;
	float visualColumnSplit;

public: //auditory effects
    float auditoryEcho;
    float auditoryBrokenRecord;
    float auditoryStatic;
    float auditoryDistortion;
    float auditorySensitivity;
    float auditoryMusicHallucination;
    float auditoryWhispersHallucination;
    float auditoryVoicesHallucination;
    float auditoryWallScratchesHallucination;
    float auditoryDoorKnocksHallucination;

public:
    static DrugEffectQuantum mix(const DrugEffectQuantum& a, const DrugEffectQuantum& b, float t);
};

}