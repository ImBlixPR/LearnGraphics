#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;
uniform vec2 resolution;

// Adjustable parameters
const float scanlineIntensity = 0.18;     // Intensity of scanlines
const float scanlineCount = 240.0;        // Number of scanlines
const float vignetteStrength = 0.2;       // Darkness around edges
const float curvatureX = 5.0;             // Horizontal curvature
const float curvatureY = 6.0;             // Vertical curvature
const float chromaticAberration = 0.006;  // RGB separation amount
const float brightness = 1.15;            // Overall brightness
const float contrast = 1.1;               // Overall contrast
const bool enableBloom = false;            // Enable bloom effect
const float bloomStrength = 0.15;         // Intensity of the bloom
const float flickerAmount = 0.03;         // Screen flicker intensity

// Apply screen curvature
vec2 curveRemapUV(vec2 uv) {
    // Convert UV from 0..1 to -1..1
    vec2 cuv = uv * 2.0 - 1.0;
    
    // Apply cubic distortion
    vec2 offset = abs(cuv.yx) / vec2(curvatureX, curvatureY);
    cuv = cuv + cuv * offset * offset;
    
    // Convert back to 0..1
    return cuv * 0.5 + 0.5;
}

// Create vignette effect
float vignette(vec2 uv) {
    uv = (uv - 0.5) * 0.95 + 0.5;
    return clamp(pow(16.0 * uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y), vignetteStrength), 0.0, 1.0);
}

// Simple bloom approximation
vec3 bloom(sampler2D tex, vec2 uv) {
    vec3 sum = vec3(0.0);
    // Sample multiple pixels in a small radius
    sum += texture(tex, uv + vec2(-1.0, -1.0) * 0.004).rgb * 0.1;
    sum += texture(tex, uv + vec2(-1.0,  1.0) * 0.004).rgb * 0.1;
    sum += texture(tex, uv + vec2( 1.0, -1.0) * 0.004).rgb * 0.1;
    sum += texture(tex, uv + vec2( 1.0,  1.0) * 0.004).rgb * 0.1;
    sum += texture(tex, uv + vec2( 0.0,  0.0) * 0.004).rgb * 0.6;
    
    // Only keep bright parts
    float luminance = dot(sum, vec3(0.2126, 0.7152, 0.0722));
    return sum * max(luminance - 0.5, 0.0) * 2.0;
}

// Random noise
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main()
{
    // Apply screen curvature
    vec2 remappedUV = curveRemapUV(TexCoords);
    
    // Check if we're outside the valid screen area
    if (remappedUV.x < 0.0 || remappedUV.x > 1.0 || 
        remappedUV.y < 0.0 || remappedUV.y > 1.0) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    
    // Apply chromatic aberration
    float r = texture(screenTexture, remappedUV + vec2(chromaticAberration, 0.0)).r;
    float g = texture(screenTexture, remappedUV).g;
    float b = texture(screenTexture, remappedUV - vec2(chromaticAberration, 0.0)).b;
    
    vec3 color = vec3(r, g, b);
    
    // Add bloom effect
    if (enableBloom) {
        vec3 bloomColor = bloom(screenTexture, remappedUV);
        color += bloomColor * bloomStrength;
    }
    
    // Apply scanlines
    float scanlineFactor = sin(remappedUV.y * scanlineCount * 3.14159);
    scanlineFactor = pow(scanlineFactor * 0.5 + 0.5, 0.5);
    color *= 1.0 - (scanlineFactor * scanlineIntensity);
    
    // Apply vignette
    color *= vignette(remappedUV);
    
    // Apply brightness & contrast
    color = (color - 0.5) * contrast + 0.5; 
    color *= brightness;
    
    // Add subtle flicker
    float flicker = 1.0 - flickerAmount * (random(vec2(time * 0.1, time * 0.1)) - 0.5);
    color *= flicker;
    
    // Add subtle noise/grain
    float noise = random(TexCoords + time * 0.01) * 0.02 - 0.01;
    color += noise;
    
    // Add subtle scanline bleeding
    if (remappedUV.y < 0.99) {
        vec3 bleed = texture(screenTexture, remappedUV + vec2(0.0, 0.002)).rgb * 0.05;
        color += bleed;
    }
    
    // Phosphor grid simulation (very subtle RGB mask)
    float mask = mod(floor(remappedUV.x * resolution.x), 3.0);
    if (mask == 0.0) color.r *= 1.05;
    else if (mask == 1.0) color.g *= 1.05;
    else color.b *= 1.05;
    
    FragColor = vec4(color, 1.0);
}