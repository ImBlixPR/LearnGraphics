#version 460 core
//#extension GL_ARB_bindless_texture : enable

out vec4 FragColor;

// Inputs from vertex shader
in vec2 fragTexCoord;  // Make sure this matches the name in your vertex shader

// Uniforms
uniform sampler2D textureSampler;
uniform vec3 colorTint = vec3(1.0, 1.0, 1.0);

void main() {
    vec4 texColor = texture(textureSampler, fragTexCoord);
    FragColor = vec4(texColor.rgb * colorTint, texColor.a);
    //FragColor = vec4(texColor);
}