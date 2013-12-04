#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace std;
using namespace glm;

GLSLProgram Object::shader;

Object::Object(void)
{
	worldMatrix = mat4(1.0f);
	worldMatrix[3] = vec4(0.0f, 0.0f, 5.0f, 1.0f);
}


Object::~Object(void)
{
}

void Object::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	//Use Program
	shader.use();
	//Set uniforms
	mat4 modelView = viewMatrix * worldMatrix;
	mat4 normalMatrix = inverse(transpose(modelView));

	/*shader.setUniform("ModelviewMatrix", modelView);
	shader.setUniform("ProjectionMatrix", projectionMatrix);
	shader.setUniform("NormalMatrix", normalMatrix);*/
	shader.setUniform("MVP", projectionMatrix * modelView);
	shader.setUniform("SolidColor", vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//For each Mesh
	for(int i = 0; i < (int)meshes.size(); i++) {
		meshes[i].Draw();
		shader.setUniform("SolidColor", vec4(1.0f, 1.0f, 1.0f, 1.0f));
		meshes[i].DrawNormals();
	}
	//Unuse Program
	shader.unuse();
}

void Object::Update(float deltaTime)
{
	//do nothing

	//update worldMatrix
}

void Object::Initialize()
{
	Mesh mesh;
	mesh.Initialize(64, 64);
	meshes.push_back(mesh);
}


void Object::InitializeShader(std::string vertexFilename, std::string fragmentFilename)
{
	//Compile Shader
	if(!shader.compileShaderFromFile(vertexFilename.c_str(), GLSLShader::VERTEX))
		cerr << "Vertex Shader failed to compile: " << shader.log() << endl;
	if(!shader.compileShaderFromFile(fragmentFilename.c_str(), GLSLShader::FRAGMENT))
		cerr << "Fragment Shader failed to compile: " << shader.log() << endl;
	if(!shader.link())
		cerr << "Shader Program failed to link: " << shader.log() << endl;
	if(!shader.validate())
		cerr << "Shader Program did not validate: " << shader.log() << endl;
}
