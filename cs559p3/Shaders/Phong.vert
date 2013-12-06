#version 420

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexture;

uniform mat4 MVP;
uniform mat3 NormalMatrix;

out vec3 Position;
out vec3 Normal;
out vec2 TextureCoord;

void main(void) {
	gl_Position = MVP * vec4(VertexPosition, 1.0);
    Position = VertexPosition;
    Normal = NormalMatrix * VertexNormal;
    TextureCoord = VertexTexture;
}