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
	mat3 normalMatrix = mat3(inverse(transpose(modelView)));

	/*shader.setUniform("ModelviewMatrix", modelView);
	shader.setUniform("ProjectionMatrix", projectionMatrix);
	shader.setUniform("NormalMatrix", normalMatrix);*/
	shader.setUniform("MVP", projectionMatrix * modelView);
	//shader.setUniform("SolidColor", vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//For each Mesh
	for(int i = 0; i < (int)meshes.size(); i++) {
		//Bind texture(s)
		switch(textures.size()) //this switch statement uses fall-through purposefully
		{
		case 4: //bind ambient map, generate dynamically
			textures[3].Bind(3);
		case 3: //bind specular map
			textures[2].Bind(2);
		case 2: //bind normal map
			textures[1].Bind(1);
		case 1: //bind color map
			textures[0].Bind(0);
			shader.setUniform("ColorMap", 0);
		}
		//Draw Mesh
		meshes[i].Draw();
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
	Mesh mesh;
	mesh.Initialize(64, 64);
	meshes.push_back(mesh);

	textures.push_back(ILContainer());
	textures[0].Initialize("Textures/hardwood_COLOR.jpg");
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
