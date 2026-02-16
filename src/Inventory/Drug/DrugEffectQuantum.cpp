#include "Inventory/Drug/DrugEffectQuantum.h"

namespace nyaa {

DrugEffectQuantum::DrugEffectQuantum()
    :durationSeconds(4)

    ,playerWalkSpeedIncrease(0)
    ,playerDiureticEffect(0)

    ,visualWavy(0)
    ,visualWorldVibration(0)
    ,visualColorShift(0)
    ,visualKeleidoscope(0)
    ,visualNoise(0)
    ,visualGreyscale(0)
    ,visualShadowPeople(0)

    ,auditoryEcho(0)
    ,auditoryBrokenRecord(0)
    ,auditoryStatic(0)
    ,auditoryDistortion(0)
    ,auditorySensitivity(0)
    ,auditoryMusicHallucination(0)
    ,auditoryWhispersHallucination(0)
    ,auditoryVoicesHallucination(0)
    ,auditoryWallScratchesHallucination(0)
    ,auditoryDoorKnocksHallucination(0)
{
}


DrugEffectQuantum DrugEffectQuantum::mix(const DrugEffectQuantum& a, const DrugEffectQuantum& b, float t)
{
    DrugEffectQuantum result;
    
    // Clamp t between 0 and 1
    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
    
    // Interpolate all fields
    result.playerWalkSpeedIncrease = a.playerWalkSpeedIncrease + (b.playerWalkSpeedIncrease - a.playerWalkSpeedIncrease) * t;
    result.playerDiureticEffect = a.playerDiureticEffect + (b.playerDiureticEffect - a.playerDiureticEffect) * t;
    
    result.visualWavy = a.visualWavy + (b.visualWavy - a.visualWavy) * t;
    result.visualWorldVibration = a.visualWorldVibration + (b.visualWorldVibration - a.visualWorldVibration) * t;
    result.visualColorShift = a.visualColorShift + (b.visualColorShift - a.visualColorShift) * t;
    result.visualKeleidoscope = a.visualKeleidoscope + (b.visualKeleidoscope - a.visualKeleidoscope) * t;
    result.visualNoise = a.visualNoise + (b.visualNoise - a.visualNoise) * t;
    result.visualGreyscale = a.visualGreyscale + (b.visualGreyscale - a.visualGreyscale) * t;
    result.visualShadowPeople = a.visualShadowPeople + (b.visualShadowPeople - a.visualShadowPeople) * t;
    
    result.auditoryEcho = a.auditoryEcho + (b.auditoryEcho - a.auditoryEcho) * t;
    result.auditoryBrokenRecord = a.auditoryBrokenRecord + (b.auditoryBrokenRecord - a.auditoryBrokenRecord) * t;
    result.auditoryStatic = a.auditoryStatic + (b.auditoryStatic - a.auditoryStatic) * t;
    result.auditoryDistortion = a.auditoryDistortion + (b.auditoryDistortion - a.auditoryDistortion) * t;
    result.auditorySensitivity = a.auditorySensitivity + (b.auditorySensitivity - a.auditorySensitivity) * t;
    result.auditoryMusicHallucination = a.auditoryMusicHallucination + (b.auditoryMusicHallucination - a.auditoryMusicHallucination) * t;
    result.auditoryWhispersHallucination = a.auditoryWhispersHallucination + (b.auditoryWhispersHallucination - a.auditoryWhispersHallucination) * t;
    result.auditoryVoicesHallucination = a.auditoryVoicesHallucination + (b.auditoryVoicesHallucination - a.auditoryVoicesHallucination) * t;
    result.auditoryWallScratchesHallucination = a.auditoryWallScratchesHallucination + (b.auditoryWallScratchesHallucination - a.auditoryWallScratchesHallucination) * t;
    result.auditoryDoorKnocksHallucination = a.auditoryDoorKnocksHallucination + (b.auditoryDoorKnocksHallucination - a.auditoryDoorKnocksHallucination) * t;
    
    return result;
}

}
