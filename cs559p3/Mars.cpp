#include "Mars.h"

#ifndef PI
#define PI 3.141592654
#endif


Mars::Mars(void)
{
}


Mars::~Mars(void)
{
}

void Mars::Initialize(float radius, float orbitRadius, float orbitSpeed, float rotationSpeed, std::string textureFile)
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
	mesh.Initialize(1440, 720);

	mesh.CreateSphereMesh();

	mesh.CreateIndices();
	mesh.CreateWireframeIndices();

	//mesh.CalculateNormals();
	mesh.FixSphereEdgeCases();
	mesh.CreateNormalsVisualization();
	mesh.CreateTextureCoords();

	LoadTexture("Textures/" + textureFile);
	name = textureFile;

	//Initialize planet's ribbon
	planetRibbon.CreateCircularRibbonControlPoints(this->radius + 10.0f, 20);
	planetRibbon.Initialize();
}