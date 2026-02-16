#pragma once
#include "Shader/Shader.h"

namespace nyaa {

class TVStaticShader : public Shader
{
public:
    TVStaticShader();
    
    void update() override;
    void setTransitionProgress(float progress);
    void setTime(float time);
    void setResolution(float resolutionX, float resolutionY);

private:
    float m_transitionProgress;
    float m_time;
    float m_resolutionX, m_resolutionY;
};
}