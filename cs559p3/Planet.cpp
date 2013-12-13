#include "Planet.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#ifndef PI
#define PI 3.141592654
#endif

Planet::Planet(void)
{
}


Planet::~Planet(void)
{
}

void Planet::Initialize(float radius, float orbitRadius, float orbitSpeed, float rotationSpeed, std::string textureFile)
{
	
	//Initialize planet with indices and mesh
	this->radius = radius;
	this->radiusOrbit = orbitRadius;
	this->orbitSpeed = orbitSpeed;
	this->rotationSpeed = rotationSpeed;
	this->scale = vec3(radius);

	currRadiansOrbit = (float)( PI );
	currRadiansRotation = 0.0f; 


	meshes.push_back(Mesh());
	Mesh & mesh = meshes[meshes.size() - 1];
	mesh.Initialize(360, 180);

	mesh.CreateSphereMesh();

	mesh.CreateIndices();
	mesh.CreateWireframeIndices();

	//mesh.CalculateNormals();
	mesh.FixSphereEdgeCases();
	mesh.CreateNormalsVisualization();
	mesh.CreateTextureCoords();

	LoadTexture("Textures/" + textureFile);
	name = textureFile;
}


void Planet::calcPosition(float deltaTime) {
	//calculate how far to travel in terms of radians
	float radians = orbitSpeed * deltaTime;
	//add radians to total radians to get position in terms of radians
	currRadiansOrbit += radians;
	if(currRadiansOrbit >= 2.0f * PI) { currRadiansOrbit = 0.0f; } //we don't want to eventually get an overflow 
	//now calculate position
	float x, z;
	x = 0 + this->radiusOrbit * cosf(currRadiansOrbit);
	z = 0 + this->radiusOrbit * sinf(currRadiansOrbit);

	planetPosition = glm::vec3(x, 0.0f, z);
}



void Planet::calcRotation(float deltaTime) {
	rotationMatrix = mat4(1.0f);

	float radians = rotationSpeed * deltaTime;
	currRadiansRotation  += radians;

	if(currRadiansRotation >= 2.0f * PI) { currRadiansRotation = 0.0f; }

	float degree = (float)( currRadiansRotation * ( 180.0f / PI ) );

	rotationMatrix = rotate(rotationMatrix, degree, vec3(0.0f, 1.0f, 0.0f));
}

void Planet::Update(float deltaTime)
{
	//find position in orbit around sun
	Planet::calcPosition(deltaTime);
	//get current amount to rotate
	Planet::calcRotation(deltaTime);

	worldMatrix = translate(mat4(1.0f), planetPosition);
	worldMatrix *= rotationMatrix;
	mat4 scaleMatrix(1.0f);
	scaleMatrix[0].x = scaleMatrix[1].y = scaleMatrix[2].z = scale.x;
	worldMatrix *= scaleMatrix;
}

void Planet::LoadTexture(std::string fileName)
{
	Mesh & mesh = meshes[0];

	mesh.textures.resize(1);
	mesh.textures[0] = new ILContainer();
	mesh.textures[0]->Initialize(fileName.c_str());
}
