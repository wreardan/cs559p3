#pragma once

#include <glm/glm.hpp>
#include <vector>

class Light
{
public:
	glm::vec3 position;
	glm::vec3 intensity;

	Light(void);
	Light(glm::vec3 pos);
	void SetPosition(glm::vec3 & position);
	glm::vec3 GetPosition();
};

class Lights
{
public:
	Lights(void);
	void Add(Light & light);

	std::vector<Light> lights;
	glm::mat4 cameraMatrix;
	glm::mat3 normalMatrix;

	int count();
	glm::vec3 GetIntensity(int i);
	glm::vec3 GetPosition(int i);
	glm::vec3 GetRawPosition(int i);
	glm::vec3 GetDirection(int i);
	glm::vec3 GetRawDirection(int i);
};

