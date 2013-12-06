#include "Lights.h"

using namespace std;
using namespace glm;

Light::Light(void)
{
	this->intensity = vec3(1.0f);
}

Light::Light(vec4 pos)
{
	this->position = pos;
	this->intensity = vec3(1.0f);
}

glm::vec3 Lights::GetIntensity(int i)
{
	vec3 pos = lights[i].intensity;
	return pos;
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

int Lights::count()
{
	return this->lights.size();
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


