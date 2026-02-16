#pragma once

namespace nyaa {

//   Name,             NormalizedHeight
#define NYAA_BODY_STACK_PARTS          \
	X(Head,            0.12f ) /*0.0*/ \
	X(Neck,            0.02f )         \
	X(ChestTop,        0.06f )         \
	X(ChestBottom,     0.19f )         \
	X(Hips,            0.08f )         \
	X(Thigh,           0.26f )         \
	X(Calf,            0.20f )         \
	X(Ankle,           0.06f )         \
	X(Foot,            0.01f ) /*1.0*/ \
	                                   \
	X(ShoulderArm,     0.17f )         \
	X(ForearmAndWrist, 0.17f )         \
	X(HandPalm,        0.05f )         \
	X(HandFinger3rd,   0.018f)         \
	X(HandThumbHalf,   0.015f)

#define X(name, nh) BodyStackPart_##name,
	
enum : int
{
    NYAA_BODY_STACK_PARTS

    BodyStackPart_Count
};

#undef X

}