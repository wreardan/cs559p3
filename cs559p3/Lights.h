#pragma once

#include <glm/glm.hpp>
#include <vector>

class Light
{
public:
	glm::vec4 position;
	glm::vec3 intensity;

	Light(void);
	Light(glm::vec4 pos);
	void SetPosition(glm::vec4 & position);
	glm::vec4 GetPosition();
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
	glm::vec4 GetPosition(int i);
	glm::vec4 GetRawPosition(int i);
	glm::vec3 GetDirection(int i);
	glm::vec3 GetRawDirection(int i);
};

