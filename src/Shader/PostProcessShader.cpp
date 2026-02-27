#include "Shader/PostProcessShader.h"

namespace nyaa {

PostProcessShader::PostProcessShader()
	:Shader("postprocess")
	,m_bloomIntensity(0)
	,m_time(0)
	,m_resolutionX(640)
	,m_resolutionY(480)
	,m_rgbDeconvergenceEffect(0)
	,m_radialBlurEffect(0)
	,m_columnSplitEffect(0)
{
}

void PostProcessShader::update()
{
	shader.setUniform("bloomIntensity", m_bloomIntensity);
	shader.setUniform("time", m_time);
	shader.setUniform("resolution", sf::Vector2f{ m_resolutionX, m_resolutionY });
	shader.setUniform("rgbDeconvergenceEffect", m_rgbDeconvergenceEffect);
	shader.setUniform("radialBlurEffect", m_radialBlurEffect);
}

void PostProcessShader::setBloomIntensity(float intensity)
{
    m_bloomIntensity = intensity;
}

void PostProcessShader::setTime(float time)
{
    m_time = time;
}

void PostProcessShader::setResolution(float resolutionX, float resolutionY)
{
    m_resolutionX = resolutionX;
    m_resolutionY = resolutionY;
}

void PostProcessShader::setRgbDeconvergenceEffect(float effect)
{
    m_rgbDeconvergenceEffect = (effect * 100) + 0.22f;
}

void PostProcessShader::setRadialBlurEffect(float effect)
{
	m_radialBlurEffect = effect * 100;
}

void PostProcessShader::setColumnSplitEffect(float effect)
{
	m_columnSplitEffect = effect;
}

}