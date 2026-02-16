#version 120
uniform float time;
uniform float drugEffect;
uniform float worldVibration;
uniform float colorShift;
uniform float worldScramblerEffect;
uniform float stretchEffect;
uniform float depthPerceptionEffect;
uniform float dissolveEffect;
uniform int numLights;
uniform vec3 lightPositions[64];
uniform vec3 lightColors[64];
uniform float lightIntensities[64];
uniform mat4 u_modelMatrix;
uniform float fogDensity;
uniform float fogStart;
uniform vec3 fogColor;
varying vec3 worldPos;
varying vec3 normal;
void main() {
    vec4 pos = gl_Vertex;
    vec4 worldPos4 = u_modelMatrix * pos;
    
    // Apply drug effects to world position
    worldPos4.y += sin(worldPos4.x * 0.1 + time * 2.0) * 2.0 * drugEffect;
    worldPos4.y += cos(worldPos4.z * 0.1 + time * 1.5) * 1.5 * drugEffect;
    worldPos4.x += sin(time * 15.0) * 0.5 * worldVibration;
    worldPos4.y += cos(time * 18.0) * 0.3 * worldVibration;
    worldPos4.z += sin(time * 12.0) * 0.4 * worldVibration;
    
    float phase = fract(sin(dot(worldPos4.xz, vec2(12.9898, 78.233))) * 43758.5453) * 6.28318;
    worldPos4.y += sin(worldPos4.x * 0.1 + time * 2.0 + phase) * worldScramblerEffect * 5.0;
    
    float distanceFromCenter = length(worldPos4.xz);
    float wave1 = sin(time * 2.0 + worldPos4.x * 8.0);
    float wave2 = sin(time * 1.5 + worldPos4.z * 6.0);
    worldPos4.x *= 1.0 + stretchEffect * wave1 * 0.5 * (1.0 + distanceFromCenter * 0.1);
    worldPos4.z *= 1.0 + stretchEffect * wave2 * 0.3 * (1.0 + distanceFromCenter * 0.1);
    
    float stutterTime = floor(time * 8.0) / 8.0;
    float depthWave = sin(worldPos4.x * 0.2 + stutterTime * 3.0) * cos(worldPos4.z * 0.15 + stutterTime * 2.0);
    worldPos4.z += depthWave * depthPerceptionEffect * 15.0;
    
    worldPos = worldPos4.xyz;

    // Transform normal using the top-left 3x3 of the model matrix
    normal = normalize((u_modelMatrix * vec4(gl_Normal, 0.0)).xyz);

    gl_Position = gl_ModelViewProjectionMatrix * worldPos4;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    
    vec4 color = gl_Color;
    float shift = sin(time * 8.0) * colorShift;
    color.r += shift * 0.3;
    color.g += cos(time * 6.0) * colorShift * 0.2;
    color.b += sin(time * 10.0) * colorShift * 0.4;
    gl_FrontColor = color;
}