#pragma once
#include "Shader/Shader.h"
#include "Game/Clock.h"

namespace nyaa {

class FireShader : public Shader
{
public:
    FireShader();
	
	virtual ~FireShader();

public:
    virtual void update() override;

public:
    Clock clock;    
};



}