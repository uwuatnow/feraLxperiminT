
#include "Inventory/Drug/CannabisTrip.h"
#include "Inventory/Drug/DrugEffectQuantum.h"
#include "Game/Util.h"
#include <cmath>

namespace nyaa {

CannabisTrip::CannabisTrip(float sativa, float indica)
{
    //Util::PrintLnFormat("CannabisTrip Constructor called.");
    
    float tripDuration = 300.0f; // 5 minutes
    
    // Initial start
    appendToTimeline(0, DrugEffectQuantum()); 
    
    // Main peak at the middle (unchanged from original intent but scaled)
    DrugEffectQuantum peak;
    peak.durationSeconds = 10.0f; 
    peak.visualKeleidoscope = 2.0f * sativa;
    peak.visualWavy = 1.0f * indica;
    peak.visualColorShift = 0.5f * sativa;
    peak.visualWorldVibration = 0.5f * sativa;
    
    appendToTimeline(tripDuration / 2.0f, peak);

    // Random medley of effects throughout the trip
    // We'll add random spikes of effects every ~20-40 seconds
    float currentTime = 10.0f;
    while (currentTime < tripDuration - 10.0f) {
        DrugEffectQuantum randomEffect;
        randomEffect.durationSeconds = 15.0f; // Each random effect lasts about 15 seconds
        
        // Randomly determine intensity based on sativa/indica influence
        // Using simplerand() equivalent 
        float intensity = (float)(std::rand() % 100) / 100.0f; 
        
        // Randomly select effects to apply
        if (std::rand() % 2 == 0) randomEffect.visualWavy = intensity * indica * 1.5f;
        if (std::rand() % 3 == 0) randomEffect.visualColorShift = intensity * sativa;
        if (std::rand() % 4 == 0) randomEffect.visualKeleidoscope = intensity * sativa * 2.0f;
        if (std::rand() % 5 == 0) randomEffect.visualWorldVibration = intensity * 0.5f;
        if (std::rand() % 6 == 0) randomEffect.visualNoise = intensity * 0.3f;
        if (std::rand() % 8 == 0) randomEffect.visualShadowPeople = intensity * 0.3f;
        // Checking DrugEffectQuantum.h again:
        // visualWavy, visualWorldVibration, visualColorShift, visualKeleidoscope, 
        // visualNoise, visualGreyscale, visualShadowPeople
        
        // Let's stick to known members
        if (std::rand() % 10 == 0) randomEffect.visualGreyscale = intensity * 0.5f;
        
        // Add to timeline
        appendToTimeline(currentTime, randomEffect);
        
        // Move forward by random amount
        currentTime += 20.0f + (float)(std::rand() % 20);
    }
    
    // End trip
    appendToTimeline(tripDuration, DrugEffectQuantum()); 
    
    //Util::PrintLnFormat("CannabisTrip configured. Final Length: %.2f", lengthSecs);
}

}
