#version 420

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexture;

uniform mat4 MVP;
uniform mat3 NormalMatrix;

layout (location = 0) out vec3 Position;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 Texture;

void main(void) {
    Position = vec3(MVP * vec4(VertexPosition, 1.0f) );
    Normal = NormalMatrix * VertexNormal;
    Texture = VertexTexture;
}