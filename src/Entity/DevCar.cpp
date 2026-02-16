#include "Entity/DevCar.h"

namespace nyaa {

DevCar::DevCar() : CarBase(
    1500.f, // mass kg
    600000.f, // inertia
    64.0f, // sizeX
    32.0f, // sizeY
    0.5f, // originX
    0.5f, // originY
    0.1f, // friction
    2.5f, // wheelBase
    { // wheels
        {22.f, -12.f, 0.f, false, true, 0.f}, // Front Left (Steerable)
        {22.f, 12.f, 0.f, false, true, 0.f},  // Front Right (Steerable)
        {-22.f, -12.f, 0.f, true, false, 0.f}, // Rear Left (Powered)
        {-22.f, 12.f, 0.f, true, false, 0.f}   // Rear Right (Powered)
    },
    1.5f, // driftDuration
    2.5f, // driftSteeringMultiplier
    32.0f, // hoodHeight
    58.0f  // roofHeight
)
{
	
}

} // namespace nyaa
