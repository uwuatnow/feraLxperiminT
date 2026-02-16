#include "Inventory/Drug/DrugEffectsManager.h"
#include "Game/Game.h"

namespace nyaa {

DrugEffectsManager::DrugEffectsManager()
    : currentTolerance(0.0f)
    , toleranceDecayRate(0.1f)  // Tolerance decays slowly when no drugs are active
    , toleranceBuildRate(0.05f)  // Tolerance builds slowly when drugs are active
    , effectDecayRate(0.8f)     // Effects decay faster when tolerance is high
    , currentEffectValue(0.0f)
{
}

DrugEffectsManager::~DrugEffectsManager()
{
}

void DrugEffectsManager::update(float deltaTime)
{
    // Update active effects
    for (auto it = activeEffects.begin(); it != activeEffects.end();) {
        it->timeRemaining -= deltaTime;
        if (it->timeRemaining <= 0.0f) {
            it = activeEffects.erase(it);
        } else {
            ++it;
        }
    }

    // Calculate total effect from all active instances
    float totalEffect = 0.0f;
    for (const auto& effect : activeEffects) {
        totalEffect += effect.effectValue;
    }

    // Clamp total effect to maximum of 1.0
    totalEffect = std::min(totalEffect, 1.0f);

    // Update tolerance based on whether effects are active
    if (totalEffect > 0.0f) {
        // Build tolerance when effects are active
        currentTolerance += toleranceBuildRate * deltaTime;
        // Don't let tolerance exceed 1.0
        currentTolerance = std::min(currentTolerance, 1.0f);
    } else {
        // Decay tolerance when no effects are active
        currentTolerance -= toleranceDecayRate * deltaTime;
        // Don't let tolerance go below 0.0
        currentTolerance = std::max(currentTolerance, 0.0f);
    }

    // Calculate the effective effect considering tolerance
    currentEffectValue = calculateEffectiveEffect();
}

void DrugEffectsManager::addEffect(float effectValue, float durationSeconds)
{
    // Clamp input values
    effectValue = std::min(std::max(effectValue, 0.0f), 1.0f);
    durationSeconds = std::max(durationSeconds, 0.0f);

    if (effectValue > 0.0f && durationSeconds > 0.0f) {
        activeEffects.push_back({
            effectValue,
            durationSeconds,
            durationSeconds
        });
    }
}

float DrugEffectsManager::getCurrentEffect() const
{
    return currentEffectValue;
}

float DrugEffectsManager::getCurrentTolerance() const
{
    return currentTolerance;
}

float DrugEffectsManager::calculateEffectiveEffect() const
{
    // Get total raw effect from all active instances
    float totalRawEffect = 0.0f;
    for (const auto& effect : activeEffects) {
        totalRawEffect += effect.effectValue;
    }
    totalRawEffect = std::min(totalRawEffect, 1.0f);

    // Apply tolerance reduction
    // The higher the tolerance, the more the effect is reduced
    float toleranceMultiplier = 1.0f - (currentTolerance * 0.8f); // Max 80% reduction at full tolerance
    
    float effectiveEffect = totalRawEffect * toleranceMultiplier;

    // Ensure the effect doesn't go negative
    return std::max(effectiveEffect, 0.0f);
}

}