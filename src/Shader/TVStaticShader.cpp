#include "Shader/TVStaticShader.h"

namespace nyaa {

TVStaticShader::TVStaticShader()
    :Shader("tv_static")
    ,m_transitionProgress(0.0f)
    ,m_time(0.0f)
    ,m_resolutionX(640.0f)
    ,m_resolutionY(480.0f)
{
}

void TVStaticShader::update()
{
    shader.setUniform("transitionProgress", m_transitionProgress);
    shader.setUniform("time", m_time);
    shader.setUniform("resolution", sf::Vector2f{ m_resolutionX, m_resolutionY });
}

void TVStaticShader::setTransitionProgress(float progress)
{
    m_transitionProgress = progress;
}

void TVStaticShader::setTime(float time)
{
    m_time = time;
}

void TVStaticShader::setResolution(float resolutionX, float resolutionY)
{
    m_resolutionX = resolutionX;
    m_resolutionY = resolutionY;
}
}