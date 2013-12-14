#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace std;
using namespace glm;

GLSLProgram Object::shader;
GLSLProgram Object::solidShader;

Object::Object(void)
{
	worldMatrix = mat4(1.0f);
	worldMatrix[3] = vec4(0.0f, 0.0f, 5.0f, 1.0f);
	wireframeMode = false;
	drawNormals = false;
	isStar = false;
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
	//shader.setUniform("MVP", projectionMatrix * modelView);
	//shader.setUniform("ModelViewMatrix", modelView);
	//shader.setUniform("NormalMatrix", normalMatrix);
	shader.setUniform("ViewMatrix", viewMatrix);
	shader.setUniform("ProjectionMatrix", projectionMatrix);
	shader.setUniform("WorldMatrix", worldMatrix);
	shader.setUniform("isStar", isStar);
	//shader.setUniform("SolidColor", vec4(1.0f, 0.0f, 0.0f, 1.0f));
	solidShader.use();
	solidShader.setUniform("MVP", projectionMatrix * modelView);

	//For each Mesh
	for(int i = 0; i < (int)meshes.size(); i++) {
		//Draw Wireframe/Mesh
		if(!wireframeMode)
		{
			shader.use();
			meshes[i].Draw(shader);
		}
		else
		{
			solidShader.use();
			solidShader.setUniform("SolidColor", vec4(0.0f, 0.0f, 1.0f, 1.0f));
			meshes[i].DrawWireframe(solidShader);
		}

		//Draw Normals
		if(drawNormals)
		{
			solidShader.use();
			solidShader.setUniform("SolidColor", vec4(1.0f, 1.0f, 1.0f, 1.0f));
			meshes[i].DrawNormals();
		}
		//Unbind texture(s)
	}
	//Unuse Program
	shader.unuse();
}

void Object::LoadTexture(std::string fileName)
{
	Mesh & mesh = meshes[0];

	mesh.textures.resize(1);
	mesh.textures[0] = new ILContainer();
	mesh.textures[0]->Initialize(fileName.c_str());
}

void Object::LoadTexture(GLuint textureHandle)
{
	Mesh & mesh = meshes[0];

	mesh.textures.resize(1);
	mesh.textures[0] = new ILContainer();
	mesh.textures[0]->il_texture_handle = textureHandle;
	mesh.textures[0]->il_handle = 1;	//kludged
}


void Object::Update(float deltaTime)
{
	//update worldMatrix
	worldMatrix = mat4(1.0f);
}

void Object::Initialize()
{
	meshes.push_back(Mesh());
	Mesh & mesh = meshes[meshes.size() - 1];
	mesh.Initialize(64, 64);

	/*vec3 v1 = vec3(1.0f, 0.0f, -2.0f);
	vec3 v2 = vec3(0.0f, 0.5f, -1.0f);
	vec3 v3 = vec3(0.0f, 1.0f, 1.0f);
	vec3 v4 = vec3(1.0f, 1.5f, 2.0f);
	mesh.CreateRibbon(v1,v2,v3,v4);*/
	mesh.CreatePlanarMesh(64, 64);

	mesh.CreateIndices();
	mesh.CreateWireframeIndices();

	//some test fucntions:
	mesh.CalculateNormals();
	mesh.CreateNormalsVisualization();
	mesh.CreateTextureCoords();
}

void Object::FreeShaders()
{
	shader.remove();
	solidShader.remove();
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

	
	//Compile Solid Shader
	if(!solidShader.compileShaderFromFile("Shaders/solid.vert", GLSLShader::VERTEX))
		cerr << "Vertex Shader failed to compile: " << solidShader.log() << endl;
	if(!solidShader.compileShaderFromFile("Shaders/solid.frag", GLSLShader::FRAGMENT))
		cerr << "Fragment Shader failed to compile: " << solidShader.log() << endl;
	if(!solidShader.link())
		cerr << "Shader Program failed to link: " << solidShader.log() << endl;
	if(!solidShader.validate())
		cerr << "Shader Program did not validate: " << solidShader.log() << endl;


}

void Object::SetWorldMatrix(glm::mat4 worldMatrix)
{
	this->worldMatrix = worldMatrix;
}
