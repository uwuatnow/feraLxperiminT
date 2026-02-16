#version 120
uniform sampler2D texture;
uniform float time;
uniform vec2 resolution;
uniform float transitionProgress; // 0.0 to 1.0

// TV static noise function
float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// CRT scanline effect
float scanlines(vec2 uv, float time) {
    float scanline = sin(uv.y * resolution.y * 0.5 + time * 10.0);
    float scanlineIntensity = 0.8 + 0.2 * scanline;
    return scanlineIntensity;
}

// TV vignette effect
float vignette(vec2 uv) {
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(uv, center);
    float vignette = smoothstep(0.4, 1.0, dist);
    return 1.0 - vignette * 0.8;
}

// TV curvature effect
vec2 barrelDistortion(vec2 uv, float amount) {
    vec2 coord = uv - 0.5;
    float dist = dot(coord, coord);
    coord += coord * dist * amount;
    return coord + 0.5;
}

void main() {
    vec2 uv = gl_TexCoord[0].st;
    
    // Apply TV curvature
    vec2 curvedUV = barrelDistortion(uv, 0.1);
    
    // Get original color
    vec4 originalColor = texture2D(texture, curvedUV);
    
    // Generate TV static noise
    float noise = rand(uv * resolution * 0.1 + time * 5.0);
    noise = mix(noise, rand(uv * resolution * 0.05 + time * 2.0), 0.5);
    
    // TV static color (grey with slight blue tint)
    vec3 staticColor = vec3(0.2, 0.2, 0.25);
    staticColor += vec3(noise * 0.3);
    
    // Add some horizontal lines for CRT effect
    float horizontalLines = sin(uv.y * resolution.y * 0.2 + time * 20.0) * 0.1;
    staticColor += vec3(horizontalLines);
    
    // TV glow effect
    float glow = pow(max(0.0, 1.0 - length(uv - 0.5) * 2.0), 4.0);
    staticColor += vec3(0.1 * glow);
    
    // Combine original with static based on transition progress
    vec3 finalColor = mix(originalColor.rgb, staticColor, transitionProgress);
    
    // Apply scanlines
    float scanlineEffect = scanlines(curvedUV, time);
    finalColor *= scanlineEffect;
    
    // Apply vignette
    float vignetteEffect = vignette(curvedUV);
    finalColor *= vignetteEffect;
    
    // TV color tint (slight green/blue)
    finalColor.r *= 0.8;
    finalColor.g *= 0.9;
    finalColor.b *= 1.1;
    
    gl_FragColor = vec4(finalColor, 1.0) * gl_Color;
}