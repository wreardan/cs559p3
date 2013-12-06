#include "Lights.h"

using namespace std;
using namespace glm;

Light::Light(void)
{
	this->intensity = vec3(1.0f);
}

Light::Light(vec3 pos)
{
	this->position = pos;
	this->intensity = vec3(1.0f);
}

glm::vec3 Lights::GetIntensity(int i)
{
	vec3 pos = lights[i].intensity;
	return pos;
}

void Light::SetPosition(glm::vec3 & position)
{
	this->position = position;
}

glm::vec3 Light::GetPosition()
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

glm::vec3 Lights::GetPosition(int i)
{
	vec3 pos = vec3(cameraMatrix * vec4(lights[i].GetPosition(), 1.0f));
	return pos;
}

glm::vec3 Lights::GetRawPosition(int i)
{
	vec3 pos = lights[i].GetPosition();
	return pos;
}


