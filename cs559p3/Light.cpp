#include "Light.h"

using namespace std;
using namespace glm;

Light::Light(void)
{
	this->direction = vec3(1.0f, 0.0f, 0.0f);
	this->lightPV = mat4(1.0f);
}


Light::~Light(void)
{
}

void Light::SetPosition(glm::vec4 & position)
{
	this->position = position;
}

glm::vec4 Light::GetPosition()
{
	return position;
}


//Light(s)
Lights::Lights()
{
}

void Lights::Add(Light & light)
{
	this->lights.push_back(light);
}

glm::vec4 Lights::GetPosition(int i)
{
	vec4 pos = cameraMatrix * lights[i].GetPosition();
	return pos;
}

glm::vec4 Lights::GetRawPosition(int i)
{
	vec4 pos = lights[i].GetPosition();
	return pos;
}

glm::vec3 Lights::GetDirection(int i)
{
	vec3 pos = normalMatrix * lights[i].direction; //Multiply by normal matrix??!!
	//vec3 pos = lights[i].direction; //Multiply by normal matrix??!!
	return pos;
}

glm::vec3 Lights::GetRawDirection(int i)
{
	vec3 pos = lights[i].direction; //Multiply by normal matrix??!!
	return pos;
}


