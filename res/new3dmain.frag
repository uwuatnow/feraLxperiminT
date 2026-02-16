#version 120
uniform sampler2D texture;
uniform float time;
uniform float colorShift;
uniform float kaleidoscopeEffect;
uniform float noiseEffect;
uniform float greyscaleEffect;
uniform float stretchEffect;
uniform float bitcrushEffect;
uniform float worldScramblerEffect;
uniform float fractalDrugEffect;
uniform float dissolveEffect;
uniform float fullbright;
uniform float daytime;
uniform int numLights;
uniform vec3 lightPositions[64];
uniform vec3 lightColors[64];
uniform float lightIntensities[64];
uniform int numHeadlights;
uniform vec3 headlightPositions[32];
uniform vec3 headlightDirections[32];
uniform vec3 headlightColors[32];
uniform float headlightIntensities[32];
uniform vec3 cameraPosition;
uniform float fogDensity;
uniform float fogStart;
uniform vec3 fogColor;
varying vec3 worldPos;
varying vec3 normal;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float squareWave(float x) {
    return sign(sin(x));
}

float triangleWave(float x) {
    return abs(fract(x / 6.28318) * 2.0 - 1.0) * 2.0 - 1.0;
}

void main() {
    vec2 pos = gl_TexCoord[0].st;
    

    
    // Kaleidoscope effect
    if (kaleidoscopeEffect > 0.0) {
        float sineWave = sin(time * 0.8);
        float triWave = triangleWave(time * 1.2);
        float fadeIntensity = (sineWave + triWave * 0.7) * 0.5;
        float effectStrength = kaleidoscopeEffect * fadeIntensity;

        // Vary the center chaotically based on position for more varied patterns
        vec2 centerOffset = vec2(
            (random(pos * 10.0) - 0.5) * 0.3,
            (random(pos * 10.0 + vec2(1.0, 1.0)) - 0.5) * 0.3
        );
        vec2 center = vec2(0.5, 0.5) + centerOffset * sin(time * 0.2) * 0.5;
        vec2 uv = pos - center;
        float angle = atan(uv.y, uv.x);
        float radius = length(uv);

        // Create kaleidoscope segments with chaotic angle offset
        float segments = 6.0 + sin(pos.x * 5.0 + time * 0.3) * 2.0;
        float angleOffset = time * 0.5 + random(pos * 8.0) * 6.28;
        angle = mod(angle + angleOffset, 3.14159 * 2.0 / segments);
        if (mod(floor(angle / (3.14159 * 2.0 / segments)), 2.0) == 1.0) {
            angle = (3.14159 * 2.0 / segments) - angle;
        }

        uv = vec2(cos(angle), sin(angle)) * radius;
        vec2 kaleidoscopePos = uv + center;
        pos = mix(pos, kaleidoscopePos, effectStrength);
    }
    
    vec4 texColor = texture2D(texture, pos) * gl_Color;
    
    float streaks = sin(pos.x * 15.0 + time * 0.8) + sin(pos.y * 12.0 + time * 0.6) + 
                    sin((pos.x - pos.y) * 8.0 + time * 0.4);
    float blobs = sin(length(pos - vec2(0.3, 0.7)) * 10.0 + time * 0.5) + 
                  sin(length(pos - vec2(0.7, 0.3)) * 8.0 + time * 0.7);
    float phaseShift = (streaks * 0.6 + blobs * 0.4) * 3.14;
    
    float contrast = 1.0 + sin(time * 7.0 + phaseShift) * colorShift * 0.5;
    texColor.rgb = (texColor.rgb - 0.5) * contrast + 0.5;
    
    texColor.r += sin(time * 5.0 + phaseShift) * colorShift * 0.3;
    texColor.g += sin(time * 5.0 + phaseShift + 2.09) * colorShift * 0.3;
    texColor.b += sin(time * 5.0 + phaseShift + 4.19) * colorShift * 0.3;
    
    // Noise effect
    if (noiseEffect > 0.0) {
        float noiseTime = floor(time * 8.0) / 8.0;
        vec2 noiseCoord = gl_TexCoord[0].st * 800.0 + noiseTime * 50.0;
        float noise = random(noiseCoord) * 2.0 - 1.0;
        texColor.rgb += noise * noiseEffect * 0.3;
    }
    
    // Greyscale darkening effect
    if (greyscaleEffect > 0.0) {
        float grey = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
        texColor.rgb = mix(texColor.rgb, vec3(grey), greyscaleEffect);
        texColor.rgb *= (1.0 - greyscaleEffect * 0.5);
    }
    
    // Bitcrush effect
    if (bitcrushEffect > 0.0) {
        float levels = mix(5.0, 1.0, pow(bitcrushEffect, 1.0));
        texColor.rgb = floor(texColor.rgb * levels) / levels;
    }
    
    // Fractal drug effect
    if (fractalDrugEffect > 0.0) {
        vec2 center = vec2(0.5, 0.5);
        vec2 uv = pos - center;
        float regionPhase = sin(pos.x * 3.0) * cos(pos.y * 2.5) * 3.14;
        float angle = atan(uv.y, uv.x) + time * 2.0;
        float radius = length(uv);
        
        float fractal = sin(radius * 20.0 + time * 3.0 + regionPhase) * cos(angle * 8.0 + time * 1.5);
        fractal += sin(radius * 35.0 - time * 2.0) * cos(angle * 12.0 - time * 2.5 + regionPhase);
        
        float stutterTime = floor((time + regionPhase * 0.3) * 15.0) / 15.0;
        float opacity = abs(sin(stutterTime * 8.0)) * abs(cos(stutterTime * 6.0 + regionPhase)) * fractalDrugEffect;
        
        vec3 fractalColor = vec3(
            sin(fractal + time * 2.0 + regionPhase) * 0.5 + 0.5,
            sin(fractal + time * 2.5 + 2.0) * 0.5 + 0.5,
            sin(fractal + time * 3.0 + 4.0 + regionPhase * 0.5) * 0.5 + 0.5
        );
        
        texColor.rgb = mix(texColor.rgb, fractalColor, opacity * 0.6);
    }
    
    // Dissolve/melt effect
    if (dissolveEffect > 0.0) {
        float noise = fract(sin(dot(pos, vec2(12.9898, 78.233))) * 43758.5453);
        float meltThreshold = dissolveEffect * (1.0 + sin(time * 2.0) * 0.3);
        if (noise < meltThreshold) {
            discard;
        }
        float edgeFade = smoothstep(meltThreshold - 0.1, meltThreshold, noise);
        texColor.rgb = mix(vec3(1.0, 0.3, 0.0), texColor.rgb, edgeFade);
    }
    
    // Smooth daylight cycle: sunrise 5am-7am, sunset 7pm-9pm
    float dayIntensity = smoothstep(5.0/24.0, 7.0/24.0, daytime) * (1.0 - smoothstep(19.0/24.0, 21.0/24.0, daytime));
    float nightIntensity = 1.0 - dayIntensity;

    // Lighting calculation
    if (fullbright > 0.5) {
        // Fullbright mode - everything is fully lit
        texColor.rgb *= vec3(1.0, 1.0, 1.0);
    } else {
        // Day: fully lit (pure white), Night: dim cool light
        vec3 dayAmbient = vec3(1.0, 1.0, 1.0) * dayIntensity;
        vec3 nightAmbient = vec3(0.05, 0.05, 0.15) * nightIntensity;
        vec3 lighting = dayAmbient + nightAmbient;
        
        vec3 norm = normalize(normal);
        for (int i = 0; i < numLights && i < 64; i++) {
            // Convert light position from absolute world coordinates to camera-relative coordinates
            vec3 cameraRelativeLightPos = lightPositions[i] - cameraPosition;
            
            // Calculate light direction from surface to light (in camera space)
            vec3 lightDir = normalize(cameraRelativeLightPos - worldPos);
            float distance = length(cameraRelativeLightPos - worldPos);
            float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
            float diffuse = max(dot(norm, lightDir), 0.0);
            lighting += lightColors[i] * lightIntensities[i] * diffuse * attenuation;
        }

        for (int i = 0; i < numHeadlights && i < 32; i++) {
            vec3 cameraRelativeHLPos = headlightPositions[i] - cameraPosition;
            vec3 toLight = cameraRelativeHLPos - worldPos;
            float distance = length(toLight);
            vec3 lightDir = normalize(toLight);
            
            // Spotlight factor: how much is the surface point within the headlight cone?
            // headlightDirections should be normalized forward vector of the car
            float cosTheta = dot(-lightDir, normalize(headlightDirections[i]));
            
            // Cone parameters: inner 0.85 (~32 deg), outer 0.5 (~60 deg)
            float spotlightEffect = smoothstep(0.5, 0.85, cosTheta);
            
            float attenuation = 1.0 / (1.0 + 0.04 * distance + 0.004 * distance * distance);
            float diffuse = max(dot(norm, lightDir), 0.0);
            
            lighting += headlightColors[i] * headlightIntensities[i] * diffuse * attenuation * spotlightEffect;
        }

        texColor.rgb *= lighting;
    }

    // Distance-based fog with time-of-day color shift
    if (fogDensity > 0.0) {
        float distance = length(worldPos - cameraPosition);
        float fogDistance = max(0.0, distance - fogStart);
        float fogFactor = 1.0 - exp(-fogDensity * fogDistance);
        
        // Darken fog at night
        vec3 nightFogColor = vec3(0.02, 0.02, 0.05);
        vec3 currentFogColor = mix(nightFogColor, fogColor, dayIntensity);
        
        texColor.rgb = mix(texColor.rgb, currentFogColor, fogFactor);
    }

    gl_FragColor = texColor;
}