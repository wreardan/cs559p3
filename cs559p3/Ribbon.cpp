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
		mesh.Initialize(16, 16);

		vec3 v1 = controlPoints[i - 1];
		vec3 v2 = controlPoints[i];
		vec3 v3 = controlPoints[i + 1];
		vec3 v4 = controlPoints[i + 2];
		mesh.CreateRibbon(v1,v2,v3,v4);
		//mesh.CreateStaircase(v1,v2,v3,v4);

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

void Ribbon::CreateCircularRibbonControlPoints(float radius, int numPoints)
{
	controlPoints.resize(0);

	float radianStep = float(2.0f * PI) / (numPoints - 3);
	for(int i = 0; i < numPoints; i++) {
		float radian = radianStep * i; //get current position

		vec3 ribbonControlPoint;
		ribbonControlPoint.x = radius * cosf(radian);
		//ribbonControlPoint.y = 0.0f; //this will be translated to the planets y value
		ribbonControlPoint.y = sinf(radian);
		ribbonControlPoint.z = radius * sinf(radian);

		controlPoints.push_back(ribbonControlPoint);
	}
}

glm::vec3 Ribbon::GetCameraPosition(float time)
{
	while(time > 1.0f)
		time -= 1.0f;
	//assert(time <= 1.0f && time >= 0.0f);
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
	//vec3 facing = catmullRom(v1, v2, v3, v4, splineTime + 0.05f);
	vec3 facing = vec3(0,0,0);
	vec3 up = vec3(0,1,0);

	return position;
	//return lookAt(position, facing, up);
}



Ribbon::~Ribbon(void)
{
}
