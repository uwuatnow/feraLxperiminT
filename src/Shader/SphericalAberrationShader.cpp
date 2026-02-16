#include "Shader/SphericalAberrationShader.h"
#include "Game/Game.h"

namespace nyaa {

SphericalAberrationShader::SphericalAberrationShader()
    :Shader("spherical_aberration")
    ,m_aberrationStrength(0.1f)
{
}

void SphericalAberrationShader::update()
{
    shader.setUniform("aberrationStrength", m_aberrationStrength);
    shader.setUniform("time", static_cast<float>(0));
}

void SphericalAberrationShader::setAberrationStrength(float strength)
{
    m_aberrationStrength = strength;
}

void SphericalAberrationShader::setScreenSize(float width, float height)
{
    shader.setUniform("screenSize", sf::Vector2f(width, height));
}

}