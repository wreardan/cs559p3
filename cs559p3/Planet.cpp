#include "Planet.h"


Planet::Planet(void)
{
}


Planet::~Planet(void)
{
}

void Planet::Initialize()
{
	
	meshes.push_back(Mesh());
	Mesh & mesh = meshes[meshes.size() - 1];
	mesh.Initialize(360, 180);

	mesh.CreateSphereMesh();

	mesh.CreateIndices();
	mesh.CreateWireframeIndices();

	mesh.CalculateNormals();
	mesh.CreateNormalsVisualization();
	mesh.CreateTextureCoords();
}

void Planet::Update(float deltaTime)
{

}

void Planet::LoadTexture(char * fileName)
{
	Mesh & mesh = meshes[0];

	mesh.textures.resize(1);
	mesh.textures[0] = new ILContainer();
	mesh.textures[0]->Initialize(fileName);
}
