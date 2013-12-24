#include "Ribbon.h"

#include <glm/gtx/spline.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;


#define PI 3.14159265359f

Ribbon::Ribbon(void)
{
	controlPoints.push_back(vec3(0.0f, 0.0f, 4.0f));
	controlPoints.push_back(vec3(0.0f, 0.5f, 3.0f));
	controlPoints.push_back(vec3(0.0f, 1.0f, 2.0f));
	controlPoints.push_back(vec3(0.0f, 1.5f, 1.0f));
	controlPoints.push_back(vec3(0.0f, 2.0f, 0.0f));
	controlPoints.push_back(vec3(0.0f, 2.5f, -1.0f));

	changing = false;
}

//void Ribbon::Initialize(void)
//{
//	textures.resize(3);
//	textures[0].Initialize("Textures/hardwood_COLOR.jpg");
//	textures[1].Initialize("Textures/hardwood_NRM.jpg");
//	textures[2].Initialize("Textures/hardwood_SPEC.jpg");
//
//	for(int i = 1; i < (int)controlPoints.size() - 2; i++)
//	{
//		meshes.push_back(Mesh());
//		Mesh & mesh = meshes[meshes.size() - 1];
//		mesh.Initialize(16, 16);
//
//		vec3 v1 = controlPoints[i - 1];
//		vec3 v2 = controlPoints[i];
//		vec3 v3 = controlPoints[i + 1];
//		vec3 v4 = controlPoints[i + 2];
//		mesh.CreateRibbon(v1,v2,v3,v4);
//		//mesh.CreateStaircase(v1,v2,v3,v4);
//
//		mesh.CreateIndices();
//		mesh.CreateWireframeIndices();
//
//		mesh.CalculateNormals();
//		mesh.CreateNormalsVisualization();
//		mesh.CreateTextureCoords();
//
//		mesh.textures.push_back(&textures[0]);
//		mesh.textures.push_back(&textures[1]);
//		mesh.textures.push_back(&textures[2]);
//	}
//}

void Ribbon::Initialize()
{

	//Create Mesh
	meshes.push_back(Mesh());
	Mesh & mesh = meshes[meshes.size() - 1];
	int numSegments = (controlPoints.size() - 3);
	int width = 16;
	int height = 16 * numSegments;
	mesh.Initialize(width, height);

	//Buffer ControlPoints, then run the FullRibbonKernel
	mesh.CreateFullRibbon(controlPoints);

	//Create other stuff inside the Mesh
	mesh.CreateIndices();
	mesh.CreateWireframeIndices();

	mesh.CalculateNormals();
	mesh.CreateNormalsVisualization();

	//Setup Texture stuff
	mesh.CreateTextureCoords(1, numSegments);
	
	textures.resize(3);
	textures[0].Initialize("Textures/hardwood_COLOR.jpg");
	textures[1].Initialize("Textures/hardwood_NRM.jpg");
	textures[2].Initialize("Textures/hardwood_SPEC.jpg");
	
	mesh.textures.push_back(&textures[0]);
	mesh.textures.push_back(&textures[1]);
	mesh.textures.push_back(&textures[2]);
}


void Ribbon::CreateCircularRibbonControlPoints(float radius, int numPoints, int mode, float deltaTime)
{
	controlPoints.resize(0);

	time += deltaTime;
	//if(time > 1.0f) time -= 1.0f;

	float radianStep = float(2.0f * PI) / (numPoints - 3);
	for(int i = 0; i < numPoints; i++) {
		float radian = radianStep * i; //get current position

		vec3 ribbonControlPoint;
		ribbonControlPoint.x = radius * cosf(radian);
		switch(mode)
		{
		case 0:
			ribbonControlPoint.y = 0.0f; //this will be translated to the planets y value
			break;
		case 1:
			changing = true;
			ribbonControlPoint.y = sinf(radian * numPoints / 2 + time);
			break;
		}
		ribbonControlPoint.z = radius * sinf(radian);

		controlPoints.push_back(ribbonControlPoint);
	}
}

glm::vec3 Ribbon::GetCameraPosition(float time)
{
	while(time > 1.0f)
		time -= 1.0f;

	int size = controlPoints.size() - 2;

	float splineTime = time * size;
	int pointNum = ( (int) splineTime) % (size);
	splineTime -= pointNum;

	int p1location = ( pointNum - 1);
	if(p1location < 0) p1location = size;
	int p2location = ( pointNum) % size;
	int p3location = ( pointNum + 1) % size;
	int p4location = ( pointNum + 2) % size;

	vec3 v1 = controlPoints[p1location];
	vec3 v2 = controlPoints[p2location];
	vec3 v3 = controlPoints[p3location];
	vec3 v4 = controlPoints[p4location];

	vec3 position = catmullRom(v1, v2, v3, v4, splineTime);

	return position;
}


Ribbon::~Ribbon(void)
{
}
void Ribbon::Update(float deltaTime)
{
	if(changing)
	{
		Mesh & mesh = meshes[0];
		mesh.CreateFullRibbon(controlPoints);
		mesh.CalculateNormals();
		mesh.CreateNormalsVisualization();
	}

	super::Update(deltaTime);
}
