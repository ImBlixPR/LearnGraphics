#version 460 core
#extension GL_ARB_bindless_texture : enable

uniform vec4 color;
out vec4 FragColor;

void main() {
    FragColor = vec4(color);
}