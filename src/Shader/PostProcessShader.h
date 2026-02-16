#pragma once
#include "Shader/Shader.h"

namespace nyaa {

class PostProcessShader : public Shader
{
public:
    PostProcessShader();
    
    void update() override;
    void setBloomIntensity(float intensity);
    void setTime(float time);
    void setResolution(float resolutionX, float resolutionY);
    void setRgbDeconvergenceEffect(float effect);
    void setRadialBlurEffect(float effect);

private:
    float m_bloomIntensity;
    float m_time;
    float m_resolutionX, m_resolutionY;
    float m_rgbDeconvergenceEffect;
    float m_radialBlurEffect;
};

}