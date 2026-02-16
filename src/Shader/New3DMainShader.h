#pragma once
#include "Shader/Shader.h"
#include <vector>

namespace nyaa {

class New3DMainShader : public Shader
{
public:
    New3DMainShader();
    
    void update() override;
    void setTime(float time);
    void setDrugEffect(float drugEffect);
    void setWorldVibration(float worldVibration);
    void setColorShift(float colorShift);
    void setKaleidoscopeEffect(float kaleidoscopeEffect);
    void setNoiseEffect(float noiseEffect);
    void setGreyscaleEffect(float greyscaleEffect);
    void setStretchEffect(float stretchEffect);
    void setBitcrushEffect(float bitcrushEffect);
    void setWorldScramblerEffect(float worldScramblerEffect);
    void setPlayerPos(float playerPosX, float playerPosY);
    void setFractalDrugEffect(float fractalDrugEffect);
    void setDepthPerceptionEffect(float depthPerceptionEffect);
    void setDissolveEffect(float dissolveEffect);
    void setFullbright(bool fullbright);
    void setDaytime(float daytime);
    void setCameraPosition(double x, double y, double z);
    void setFog(float density, float start, float colorR, float colorG, float colorB);
    
    struct Light {
        double positionX, positionY, positionZ;
        float colorR, colorG, colorB;
        float intensity;
    };
    
    struct Headlight {
        double positionX, positionY, positionZ;
        double directionX, directionY, directionZ;
        float colorR, colorG, colorB;
        float intensity;
    };
    
    Light& addLight();
    void removeLight(int id);
    void clearLights();
    
    void addHeadlight(const Headlight& headlight);
    void clearHeadlights();

    int addPlayerFollowLight(float colorR = 1.0f, float colorG = 1.0f, float colorB = 0.8f, float intensity = 1.0f);
    int getLightCount() const { return (int)m_lights.size(); }
    std::map<int, Light>& getLights() { return m_lights; }
    void getPlayerPos(float* outX, float* outY) const;

private:
    float m_time;
    float m_drugEffect;
    float m_worldVibration;
    float m_colorShift;
    float m_kaleidoscopeEffect;
    float m_noiseEffect;
    float m_greyscaleEffect;
    float m_stretchEffect;
    float m_bitcrushEffect;
    float m_worldScramblerEffect;
    float m_playerPosX, m_playerPosY;
    float m_fractalDrugEffect;
    float m_depthPerceptionEffect;
    float m_dissolveEffect;
    bool m_fullbright;
    float m_daytime;
    double m_cameraPosX, m_cameraPosY, m_cameraPosZ;
    float m_fogDensity;
    float m_fogStart;
    float m_fogColorR, m_fogColorG, m_fogColorB;
    std::map<int, Light> m_lights;
    std::vector<Headlight> m_headlights;
    int m_nextLightId;
    int m_playerFollowLightId;
};

}