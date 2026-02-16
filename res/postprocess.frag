#version 120
uniform sampler2D texture;
uniform float bloomIntensity;
uniform float rgbDeconvergenceEffect;
uniform float radialBlurEffect;
uniform float time;
uniform vec2 resolution;

// High-quality bloom with multiple blur passes and vignette
vec4 blur(sampler2D tex, vec2 uv, vec2 direction, float strength) {
    vec4 color = vec4(0.0);
    vec2 texelSize = 1.0 / resolution;
    
    // Gaussian blur weights for smooth bloom
    float weights[9];
    weights[0] = 0.227027;
    weights[1] = 0.1945946;
    weights[2] = 0.1216216;
    weights[3] = 0.054054;
    weights[4] = 0.016216;
    weights[5] = 0.016216;
    weights[6] = 0.054054;
    weights[7] = 0.1216216;
    weights[8] = 0.1945946;
    
    color += texture2D(tex, uv) * weights[0];
    
    for(int i = 1; i < 9; i++) {
        vec2 offset = direction * texelSize * float(i) * strength;
        color += texture2D(tex, uv + offset) * weights[i];
        color += texture2D(tex, uv - offset) * weights[i];
    }
    
    return color;
}

vec4 extractBright(vec4 color, float threshold) {
    float brightness = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    return color * smoothstep(threshold - 0.1, threshold + 0.1, brightness);
}

// Helper to get color with RGB Deconvergence applied
vec4 getSourceColor(vec2 uv) {
    if (rgbDeconvergenceEffect > 0.0) {
        float offset = rgbDeconvergenceEffect * 0.005; 
        vec2 rOffset = vec2(offset, 0.0);
        vec2 bOffset = vec2(-offset, 0.0);
        
        float r = texture2D(texture, uv + rOffset).r;
        float g = texture2D(texture, uv).g;
        float b = texture2D(texture, uv + bOffset).b;
        float a = texture2D(texture, uv).a;
        
        return vec4(r, g, b, a);
    } else {
        return texture2D(texture, uv);
    }
}

void main() {
    vec2 uv = gl_TexCoord[0].st;
    
    vec4 originalColor = getSourceColor(uv);

    if (radialBlurEffect > 0.0) {
        vec2 center = vec2(0.5, 0.5);
        vec2 toCenter = center - uv;
        float dist = length(toCenter);
        vec2 dir = toCenter / dist;
        int samples = 10;
        float strength = radialBlurEffect * 0.05; // tunable strength
        
        vec4 blurColor = vec4(0.0);
        for (int i = 0; i < samples; i++) {
            float scale = 1.0 - strength * (float(i) / float(samples - 1)) * dist; // scale based on distance from center
            vec2 sampleUV = center - (center - uv) * scale;
            blurColor += getSourceColor(sampleUV);
        }
        blurColor /= float(samples);
        originalColor = mix(originalColor, blurColor, radialBlurEffect > 0.5 ? 1.0 : radialBlurEffect * 2.0); // Basic mix
        // Better radial blur usually just replaces, but let's accumulate
        originalColor = blurColor;
    }
    
    if (bloomIntensity <= 0.0) {
        gl_FragColor = originalColor * gl_Color;
        return;
    }
    
    // Extract bright areas for bloom
    vec4 brightColor = extractBright(originalColor, 0.6);
    
    // Multi-pass blur for smooth, cloudy bloom
    vec4 bloom1 = blur(texture, uv, vec2(1.0, 0.0), 2.0 + sin(time * 0.5) * 0.3);
    vec4 bloom2 = blur(texture, uv, vec2(0.0, 1.0), 2.0 + cos(time * 0.7) * 0.3);
    vec4 bloom3 = blur(texture, uv, vec2(0.707, 0.707), 3.0 + sin(time * 0.3) * 0.5);
    vec4 bloom4 = blur(texture, uv, vec2(-0.707, 0.707), 3.0 + cos(time * 0.4) * 0.5);
    
    // Combine bloom passes for cloudy effect
    vec4 combinedBloom = (bloom1 + bloom2 + bloom3 + bloom4) * 0.25;
    combinedBloom = extractBright(combinedBloom, 0.4);
    
    // Add subtle color variation for more organic look
    combinedBloom.r += sin(time * 0.8 + uv.x * 3.14) * 0.05 * bloomIntensity;
    combinedBloom.g += sin(time * 0.6 + uv.y * 3.14) * 0.05 * bloomIntensity;
    combinedBloom.b += sin(time * 0.4 + (uv.x + uv.y) * 3.14) * 0.05 * bloomIntensity;
    
    // Vignette effect
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(uv, center);
    float vignette = 1.0 - smoothstep(0.3, 0.8, dist);
    vignette = mix(0.7, 1.0, vignette); // Subtle vignette
    
    // Apply bloom with intensity control
    vec4 finalColor = originalColor + combinedBloom * bloomIntensity * 1.5;
    finalColor.rgb *= vignette;
    
    // Enhance contrast slightly for better bloom visibility
    finalColor.rgb = (finalColor.rgb - 0.5) * (1.0 + bloomIntensity * 0.2) + 0.5;
    
    gl_FragColor = finalColor * gl_Color;
}