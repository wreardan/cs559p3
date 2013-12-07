#include "Ribbon.h"

using namespace std;
using namespace glm;

Ribbon::Ribbon(void)
{
	controlPoints.push_back(vec3(1.0f, 0.0f, -2.0f));
	controlPoints.push_back(vec3(0.0f, 0.5f, -1.0f));
	controlPoints.push_back(vec3(0.0f, 1.0f, 1.0f));
	controlPoints.push_back(vec3(1.0f, 1.5f, 2.0f));
	controlPoints.push_back(vec3(2.0f, 2.0f, 3.0f));
	controlPoints.push_back(vec3(3.0f, 2.5f, 4.0f));
}

void Ribbon::Initialize(void)
{
	textures.resize(3);
	textures[0].Initialize("Textures/hardwood_COLOR.jpg");
	textures[1].Initialize("Textures/hardwood_NRM.jpg");
	textures[2].Initialize("Textures/hardwood_SPEC.jpg");

	for(int i = 1; i < controlPoints.size() - 2; i++)
	{
		meshes.push_back(Mesh());
		Mesh & mesh = meshes[meshes.size() - 1];
		mesh.Initialize(64, 64);

		vec3 v1 = controlPoints[i - 1];
		vec3 v2 = controlPoints[i];
		vec3 v3 = controlPoints[i + 1];
		vec3 v4 = controlPoints[i + 2];
		mesh.CreateRibbon(v1,v2,v3,v4);

		mesh.CreateIndices();
		mesh.CreateWireframeIndices();

		mesh.CalculateNormals();
		mesh.CreateNormalsVisualization();
		mesh.CreateTextureCoords();

		mesh.textures.push_back(&textures[0]);
		mesh.textures.push_back(&textures[1]);
		mesh.textures.push_back(&textures[2]);
	}
}

Ribbon::~Ribbon(void)
{
}
