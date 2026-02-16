#include "Shader/New3DMainShader.h"
#include "Game/Game.h"
#include <cassert>

namespace nyaa {

New3DMainShader::New3DMainShader()
    :Shader("new3dmain")
    ,m_time(0.0f)
    ,m_drugEffect(0.0f)
    ,m_worldVibration(0.0f)
    ,m_colorShift(0.0f)
    ,m_kaleidoscopeEffect(0.0f)
    ,m_noiseEffect(0.0f)
    ,m_greyscaleEffect(0.0f)
    ,m_stretchEffect(0.0f)
    ,m_bitcrushEffect(0.0f)
    ,m_worldScramblerEffect(0.0f)
    ,m_playerPosX(0.0f)
    ,m_playerPosY(0.0f)
    ,m_fractalDrugEffect(0.0f)
    ,m_depthPerceptionEffect(0.0f)
    ,m_dissolveEffect(0.0f)
    ,m_fullbright(false)
    ,m_daytime(0.5f)
    ,m_cameraPosX(0.0)
    ,m_cameraPosY(0.0)
    ,m_cameraPosZ(0.0)
    ,m_fogDensity(0.0f)
    ,m_fogStart(0.0f)
    ,m_fogColorR(0.5f)
    ,m_fogColorG(0.5f)
    ,m_fogColorB(0.5f)
    ,m_nextLightId(0)
    ,m_playerFollowLightId(-1)
{

    // addLight(Light{sf::Vector3f{0.f,0.f,0.f}, sf::Vector3f{1.f,1.f,1.f}, 1.f});
    // addLight(Light{sf::Vector3f{15.f * 16.f,15.f * 16.f,1.f}, sf::Vector3f{0.f,1.f,1.f}, 5.f});
}

void New3DMainShader::update()
{
    shader.setUniform("time", m_time);
    shader.setUniform("drugEffect", m_drugEffect);
    shader.setUniform("worldVibration", m_worldVibration);
    shader.setUniform("colorShift", m_colorShift);
    shader.setUniform("kaleidoscopeEffect", m_kaleidoscopeEffect);
    shader.setUniform("noiseEffect", (m_noiseEffect * 3) + 0.2f);
    shader.setUniform("greyscaleEffect", m_greyscaleEffect);
    shader.setUniform("stretchEffect", m_stretchEffect);
    shader.setUniform("bitcrushEffect", m_bitcrushEffect);
    shader.setUniform("worldScramblerEffect", m_worldScramblerEffect);
    //shader.setUniform("playerPos", sf::Vector2f{ m_playerPosX, m_playerPosY });
    shader.setUniform("fractalDrugEffect", m_fractalDrugEffect);
    shader.setUniform("depthPerceptionEffect", m_depthPerceptionEffect);
    shader.setUniform("dissolveEffect", m_dissolveEffect);
    shader.setUniform("fullbright", m_fullbright ? 1.0f : 0.0f);
    shader.setUniform("daytime", m_daytime);
    // Use double precision for camera position to maintain accuracy at large distances
    shader.setUniform("cameraPosition", sf::Vector3f{(float)m_cameraPosX, (float)m_cameraPosY, (float)m_cameraPosZ});
    shader.setUniform("fogDensity", m_fogDensity);
    shader.setUniform("fogStart", m_fogStart);
    shader.setUniform("fogColor", sf::Vector3f{m_fogColorR, m_fogColorG, m_fogColorB});
    
    // Update lights
    int numLights = std::min((int)m_lights.size(), 64);
    shader.setUniform("numLights", numLights);
    
    int i = 0;
    for (const auto& pair : m_lights)
    {
        if (i >= 64) {
            assert(false); //light limit exceeded
            break;
        }
        const Light& light = pair.second;
        shader.setUniform("lightPositions[" + std::to_string(i) + "]", sf::Vector3f{ (float)light.positionX, (float)light.positionY, (float)light.positionZ });
        shader.setUniform("lightColors[" + std::to_string(i) + "]", sf::Vector3f{ light.colorR, light.colorG, light.colorB });
        shader.setUniform("lightIntensities[" + std::to_string(i) + "]", light.intensity);
        i++;
    }

    // Update headlights
    int numHeadlights = std::min((int)m_headlights.size(), 32);
    shader.setUniform("numHeadlights", numHeadlights);
    for (int j = 0; j < numHeadlights; ++j)
    {
        const Headlight& hl = m_headlights[j];
        std::string prefix = "headlightPositions[" + std::to_string(j) + "]";
        shader.setUniform(prefix, sf::Vector3f{ (float)hl.positionX, (float)hl.positionY, (float)hl.positionZ });
        
        prefix = "headlightDirections[" + std::to_string(j) + "]";
        shader.setUniform(prefix, sf::Vector3f{ (float)hl.directionX, (float)hl.directionY, (float)hl.directionZ });
        
        prefix = "headlightColors[" + std::to_string(j) + "]";
        shader.setUniform(prefix, sf::Vector3f{ hl.colorR, hl.colorG, hl.colorB });
        
        prefix = "headlightIntensities[" + std::to_string(j) + "]";
        shader.setUniform(prefix, hl.intensity);
    }

    shader.setUniform("u_modelMatrix", sf::Glsl::Mat4(sf::Transform::Identity));
}

void New3DMainShader::setTime(float time)
{
    m_time = time;
}

void New3DMainShader::setDrugEffect(float drugEffect)
{
    m_drugEffect = drugEffect;
}

void New3DMainShader::setWorldVibration(float worldVibration)
{
    m_worldVibration = worldVibration;
}

void New3DMainShader::setColorShift(float colorShift)
{
    m_colorShift = colorShift;
}

void New3DMainShader::setKaleidoscopeEffect(float kaleidoscopeEffect)
{
    m_kaleidoscopeEffect = kaleidoscopeEffect;
}

void New3DMainShader::setNoiseEffect(float noiseEffect)
{
    m_noiseEffect = noiseEffect;
}

void New3DMainShader::setGreyscaleEffect(float greyscaleEffect)
{
    m_greyscaleEffect = greyscaleEffect;
}

void New3DMainShader::setStretchEffect(float stretchEffect)
{
    m_stretchEffect = stretchEffect / 10.f;
}

void New3DMainShader::setBitcrushEffect(float bitcrushEffect)
{
    m_bitcrushEffect = bitcrushEffect;
}

void New3DMainShader::setWorldScramblerEffect(float worldScramblerEffect)
{
    m_worldScramblerEffect = worldScramblerEffect / 3.0f;
}

void New3DMainShader::setPlayerPos(float playerPosX, float playerPosY)
{
    m_playerPosX = playerPosX;
    m_playerPosY = playerPosY;

    // Update player-following light position (relative to camera)
    if (m_playerFollowLightId != -1 && m_lights.find(m_playerFollowLightId) != m_lights.end()) {
        auto& light = m_lights[m_playerFollowLightId];
        // Use double precision for accurate player position calculations at large distances
        light.positionX = playerPosX / 16.0 - m_cameraPosX;
        light.positionY = 3.0f;
        light.positionZ = (playerPosY / 16.0) - m_cameraPosZ + 1.0f;
    }
}

void New3DMainShader::setFractalDrugEffect(float fractalDrugEffect)
{
    m_fractalDrugEffect = fractalDrugEffect;
}

void New3DMainShader::setDepthPerceptionEffect(float depthPerceptionEffect)
{
    m_depthPerceptionEffect = depthPerceptionEffect / 10;
}

void New3DMainShader::setDissolveEffect(float dissolveEffect)
{
    m_dissolveEffect = dissolveEffect;
}

void New3DMainShader::setFullbright(bool fullbright)
{
    m_fullbright = fullbright;
}

void New3DMainShader::setDaytime(float daytime)
{
    m_daytime = daytime;
}

void New3DMainShader::setCameraPosition(double x, double y, double z)
{
    m_cameraPosX = x;
    m_cameraPosY = y;
    m_cameraPosZ = z;
}

void New3DMainShader::setFog(float density, float start, float colorR, float colorG, float colorB)
{
    m_fogDensity = density;
    m_fogStart = start;
    m_fogColorR = colorR;
    m_fogColorG = colorG;
    m_fogColorB = colorB;
}

New3DMainShader::Light& New3DMainShader::addLight()
{
    int id = m_nextLightId++;
    m_lights[id] = Light {
        0.0, 0.0, 0.0, // pos (double)
        0.f, 0.f, 0.f, // col
        0.f // intensity
    };
    return m_lights[id];
}

void New3DMainShader::removeLight(int id)
{
    m_lights.erase(id);
}

void New3DMainShader::clearLights()
{
    m_lights.clear();
    m_nextLightId = 0;
    m_playerFollowLightId = -1;
}

void New3DMainShader::addHeadlight(const Headlight& headlight)
{
    m_headlights.push_back(headlight);
}

void New3DMainShader::clearHeadlights()
{
    m_headlights.clear();
}

int New3DMainShader::addPlayerFollowLight(float colorR, float colorG, float colorB, float intensity)
{
    if (m_playerFollowLightId != -1) {
        removeLight(m_playerFollowLightId);
    }
    
    Light& light = addLight();
    light.colorR = colorR;
    light.colorG = colorG;
    light.colorB = colorB;
    light.intensity = intensity;
    //2d XY -> 3d XZ (aka 2.5d), relative to camera
    // Use double precision for accurate player position calculations at large distances
    light.positionX = m_playerPosX / 16.0 - m_cameraPosX;
    light.positionY = 3.0f;
    light.positionZ = m_playerPosY / 16.0 - m_cameraPosZ;
    
    m_playerFollowLightId = m_nextLightId - 1;
    return m_playerFollowLightId;
}

void New3DMainShader::getPlayerPos(float* outX, float* outY) const
{
    if (outX) *outX = m_playerPosX;
    if (outY) *outY = m_playerPosY;
}

}