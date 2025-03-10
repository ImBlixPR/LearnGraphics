#version 460
layout (location = 0) in vec2 vPos;
layout (location = 2) in vec2 texCoordinate;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 fragTexCoord;

void main() {
	  gl_Position = projection * view * model * vec4(vPos, 0.0, 1.0);
	  fragTexCoord = texCoordinate;
}