#pragma once
#include "Shader/Shader.h"

namespace nyaa {

class SphericalAberrationShader : public Shader
{
public:
    SphericalAberrationShader();
    
    void update() override;
    void setAberrationStrength(float strength);
    void setScreenSize(float width, float height);

private:
    float m_aberrationStrength;
};

}