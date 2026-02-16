#version 330 core

uniform sampler2D texture;
uniform vec2 resolution;
uniform float aberrationStrength;
uniform float time;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    vec2 uv = texCoord;
    vec2 center = vec2(0.5, 0.5);
    vec2 offset = uv - center;
    float dist = length(offset);
    
    // Spherical aberration coefficients
    float r_coeff = 1.0 + aberrationStrength * dist * dist;
    float g_coeff = 1.0 + aberrationStrength * dist * dist * 0.8;
    float b_coeff = 1.0 + aberrationStrength * dist * dist * 0.6;
    
    // Sample RGB channels with different distortions
    float r = texture2D(texture, center + offset * r_coeff).r;
    float g = texture2D(texture, center + offset * g_coeff).g;
    float b = texture2D(texture, center + offset * b_coeff).b;
    
    fragColor = vec4(r, g, b, 1.0);
}