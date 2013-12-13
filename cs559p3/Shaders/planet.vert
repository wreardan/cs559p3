#version 400


layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexture;

uniform mat4 WorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;



out vec3 position;
out vec3 normal;
out vec2 UV;

out vec3 positionWorld;


void main() {
	vec4 pos = ProjectionMatrix * ViewMatrix * WorldMatrix * vec4(VertexPosition, 1.0f);
	gl_Position = pos;


	mat4 modelViewMatrix = ViewMatrix * WorldMatrix;

	position = vec3(modelViewMatrix * vec4(VertexPosition, 1.0f));

	
	mat3 normalMatrix = inverse(transpose( mat3(WorldMatrix) ));
	normal = normalize( normalMatrix * VertexNormal );

	UV = VertexTexture;

	//position of the vertex in world space
	positionWorld = vec3( WorldMatrix * vec4(VertexPosition, 1.0f) ); 
}