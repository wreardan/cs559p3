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

void Object::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, Lights lights)
{
	//Use Program
	shader.use();
	//Set uniforms
	mat4 modelView = viewMatrix * worldMatrix;
	mat3 normalMatrix = inverse(transpose(mat3(viewMatrix)));
	//Set Lights
	for(int i = 0; i < lights.count(); i++) {
		shader.setUniform(shader.getUniformBlockName("Light", i, "Position").c_str(), lights.GetRawPosition(i));
		shader.setUniform(shader.getUniformBlockName("Light", i, "Intensity").c_str(), lights.GetIntensity(i));
	}

	/*shader.setUniform("ModelviewMatrix", modelView);
	shader.setUniform("ProjectionMatrix", projectionMatrix);*/
	shader.setUniform("MVP", projectionMatrix * modelView);
	shader.setUniform("NormalMatrix", normalMatrix);
	//shader.setUniform("SolidColor", vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//For each Mesh
	for(int i = 0; i < (int)meshes.size(); i++) {
		//Draw Mesh
		meshes[i].Draw(shader);
		//Draw Normals
		//shader.setUniform("SolidColor", vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//meshes[i].DrawNormals();
		//Unbind texture(s)
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
	meshes.push_back(Mesh());
	Mesh & mesh = meshes[meshes.size() - 1];
	mesh.Initialize(64, 64);

	vec3 v1 = vec3(1.0f, 0.0f, -2.0f);
	vec3 v2 = vec3(0.0f, 0.5f, -1.0f);
	vec3 v3 = vec3(0.0f, 1.0f, 1.0f);
	vec3 v4 = vec3(1.0f, 1.5f, 2.0f);
	mesh.CreateRibbon(v1,v2,v3,v4);

	mesh.CreateIndices();
	mesh.CreateWireframeIndices();

	//some test fucntions:
	mesh.CalculateNormals();
	mesh.CreateNormalsVisualization();
	mesh.CreateTextureCoords();
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
