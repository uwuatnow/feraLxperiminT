#include "Inventory/Drug/PetrolTrip.h"

namespace nyaa {

PetrolTrip::PetrolTrip()
{
    float tripDurationSecs = 30.0f;
    
    appendToTimeline(0, DrugEffectQuantum());
    
	DrugEffectQuantum peak;
	peak.durationSeconds = tripDurationSecs / 2.0f;
	peak.visualWavy = 1.0f;
	peak.visualColumnSplit = 0.7f;
	appendToTimeline(tripDurationSecs / 2.0f, peak);
    DrugEffectQuantum end;
    end.durationSeconds = tripDurationSecs;
    end.visualWorldVibration = 1.0f;
    appendToTimeline(tripDurationSecs, end);
}

PetrolTrip::~PetrolTrip()
{
}

}
