#include "Inventory/Drug/DrugTrip.h"
#include "Game/Timer.h"
#include "Game/Game.h"
#include "Game/Util.h"
#include "Map/New3DRenderer.h"
#include "Screen/InGameScreen.h"

namespace nyaa {

DrugTrip::DrugTrip()
    :tripTimer(new Timer())
    ,finished(false)
    ,lengthSecs(0.0f)
{
}

DrugTrip::~DrugTrip()
{
    delete tripTimer;
}

void DrugTrip::update()
{
    if(finished) return;
    
    tripTimer->update();
    
    // Utilize Game global for frame delta to ensure effects persist correctly for this frame
    float frameDuration = 0.016f; // Default if G is not available
    if (G) {
        frameDuration = G->frameDeltaMillis / 1000.0f;
    }
    // Add a small buffer to prevent flickering
    float effectDuration = frameDuration * 2.5f;

    // Apply effects based on timeline with interpolation
    float currentTime = tripTimer->secs();
    
    // Util::PrintLnFormat("DrugTrip Update: Time: %.2f / %.2f", currentTime, lengthSecs);

    
    DrugEffectQuantum interpolatedEffect;
    bool found = false;

    // Find the interval in the timeline
    auto it = tripTimeline.lower_bound(currentTime);

    if (it == tripTimeline.end()) {
        if (!tripTimeline.empty()) {
            // Check if we are past the last element but within lengthSecs?
            // Usually we should be done, but if not, hold the last value?
            // For now, if we are past end, we assume 0 or last value depending on logic.
            // But since CannabisTrip ends with an empty struct at lengthSecs, it should fade to 0.
        }
    } else {
        if (it == tripTimeline.begin()) {
            // Before or exactly at starts
            if (it->first <= currentTime + 0.001f) {
                 interpolatedEffect = it->second;
                 found = true;
            }
        } else {
            // Interpolate between prev and current
            auto prev = std::prev(it);
            float t0 = prev->first;
            float t1 = it->first;
            
            if (t1 > t0) {
                float t = (currentTime - t0) / (t1 - t0);
                interpolatedEffect = DrugEffectQuantum::mix(prev->second, it->second, t);
                found = true;
            }
        }
    }

    if (found) {
        // Apply visual effects to the renderer
        if (IGS && IGS->renderer) {
            New3DRenderer* renderer3D = dynamic_cast<New3DRenderer*>(IGS->renderer);
            if (renderer3D) {
                // Apply visual effects with short duration to avoid stacking indefinitely
                if (interpolatedEffect.visualWavy > 0.0f) {
                    renderer3D->addWavyWorldEffect(interpolatedEffect.visualWavy, effectDuration);
                }
                
                if (interpolatedEffect.visualWorldVibration > 0.0f) {
                    renderer3D->addWorldVibrationEffect(interpolatedEffect.visualWorldVibration, effectDuration);
                }
                
                if (interpolatedEffect.visualColorShift > 0.0f) {
                    renderer3D->addColorShiftEffect(interpolatedEffect.visualColorShift, effectDuration);
                }
                
                if (interpolatedEffect.visualKeleidoscope > 0.0f) {
                    renderer3D->addKaleidoscopeEffect(interpolatedEffect.visualKeleidoscope, effectDuration);
                }
                
                if (interpolatedEffect.visualNoise > 0.0f) {
                    renderer3D->addNoiseEffect(interpolatedEffect.visualNoise, effectDuration);
                }
                
                if (interpolatedEffect.visualGreyscale > 0.0f) {
                    renderer3D->addGreyscaleEffect(interpolatedEffect.visualGreyscale, effectDuration);
                }
            }
        }
    }

    if(tripTimer->once(lengthSecs * 1000.0f))
    {
        finished = true;
    }
}

void DrugTrip::appendToTimeline(float atSecond, DrugEffectQuantum effect)
{
    tripTimeline[atSecond] = effect;
    if(atSecond + effect.durationSeconds > lengthSecs)
    {
        lengthSecs = atSecond + effect.durationSeconds;
    }
}

float DrugTrip::getLengthSecs()
{
    return lengthSecs;
}

}