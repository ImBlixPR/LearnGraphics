#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;
uniform vec2 resolution;

// Noise function
float noise(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = TexCoords;
    
    // 1. Vertical jitter/shake (tracking errors)
    float jitterAmount = 0.0007;
    float verticalShake = jitterAmount * sin(time * 15.0 + uv.y * 10.0);
    uv.y += verticalShake;
    
    // 2. Horizontal distortion (waviness)
    float waveAmount = 0.002;
    float wave = waveAmount * sin(time + uv.y * 80.0);
    uv.x += wave;
    
    // 3. RGB split/chromatic aberration
    float rgbSplitAmount = 0.005;
    vec2 rOffset = vec2(rgbSplitAmount, 0.0);
    vec2 gOffset = vec2(0.0, 0.0);
    vec2 bOffset = vec2(-rgbSplitAmount, 0.0);
    
    float r = texture(screenTexture, uv + rOffset).r;
    float g = texture(screenTexture, uv + gOffset).g;
    float b = texture(screenTexture, uv + bOffset).b;
    
    // 4. Scanlines
    float scanlineIntensity = 0.1;
    float scanline = sin(uv.y * resolution.y * 1.0) * 0.5 + 0.5;
    scanline = pow(scanline, 1.5) * scanlineIntensity;
    
    // 5. Noise and static
    float noiseAmount = 0.1;
    float staticNoise = noise(uv * vec2(time * 10.0)) * noiseAmount;
    
    // 6. Random horizontal displacement (tracking errors)
    float trackingErrorProb = 0.01; // Probability of a tracking error
    float trackingError = 0.0;
    if (noise(vec2(time * 0.5, uv.y * 0.1)) < trackingErrorProb) {
        trackingError = (noise(vec2(time * 10.0, uv.y)) - 0.5) * 0.1;
    }
    uv.x += trackingError;
    
    // 7. VHS color distortion - slightly shift colors
    vec3 color = vec3(r, g, b);
    
    // Add scanlines, noise and slightly darken
    color = color * (1.0 - scanline);
    color += staticNoise;
    color *= 0.9; // Slightly darken
    
    // 8. Occasional horizontal line (tracking problem)
    float lineProb = 0.005;
    if (noise(vec2(time * 0.5, floor(uv.y * 100.0))) < lineProb) {
        color += 0.5;
    }
    
    FragColor = vec4(color, 1.0);
}