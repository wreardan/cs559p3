#version 420

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexture;

uniform mat4 MVP;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 WorldMatrix;

out vec3 Position;
out vec3 Normal;
out vec2 TextureCoord;
out vec3 WorldPosition;

void main(void) {
	gl_Position = MVP * vec4(VertexPosition, 1.0);
	Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0) );
    WorldPosition = vec3(WorldMatrix * vec4(VertexPosition, 1.0) );
    Normal = NormalMatrix * VertexNormal;
    TextureCoord = VertexTexture;
}