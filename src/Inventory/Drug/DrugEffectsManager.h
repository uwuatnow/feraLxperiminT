#pragma once
#include "Game/Globals.h"
#include <vector>

namespace nyaa {

class DrugEffectsManager
{
public:
    DrugEffectsManager();
    ~DrugEffectsManager();

    // Update all effects - this is called every frame
    void update(float deltaTime);

    // Add an effect from a drug trip
    void addEffect(float effectValue, float durationSeconds);

    // Get the current normalized effect value (0.0 - 1.0)
    float getCurrentEffect() const;

    // Get the current tolerance level (0.0 - 1.0)
    float getCurrentTolerance() const;

private:
    struct EffectInstance {
        float effectValue;      // The strength of this effect (0.0 - 1.0)
        float duration;         // How long this effect lasts in seconds
        float timeRemaining;    // Time remaining for this effect
    };

    std::vector<EffectInstance> activeEffects;
    
    float currentTolerance;     // Current tolerance level (0.0 - 1.0)
    float toleranceDecayRate;   // How fast tolerance decreases when no effects are active
    float toleranceBuildRate;   // How fast tolerance builds when effects are active
    float effectDecayRate;      // How fast effects decay when tolerance is high

    // Internal normalized effect value (0.0 - 1.0)
    float currentEffectValue;

    // Calculate the effective effect considering tolerance
    float calculateEffectiveEffect() const;
};

}