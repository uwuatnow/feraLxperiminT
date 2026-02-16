#pragma once
#include "Game/Globals.h"
#include "Entity/Prop.h"

namespace nyaa {

class Light : public Prop
{
public:
    Light();

	virtual ~Light();
    
    virtual void update() override;
    
public:
    unsigned char colorR, colorG, colorB;
    float intensity;
    int lightId;
};

} // namespace nyaa