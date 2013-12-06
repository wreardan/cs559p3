#pragma once

#include <glm/glm.hpp>
#include <vector>

class Light
{
protected:
public:
	glm::vec4 position;
	glm::vec3 direction;
	glm::mat4 lightPV;

	Light(void);
	void SetPosition(glm::vec4 & position);
	glm::vec4 GetPosition();
	~Light(void);
};

class Lights
{
public:
	Lights();
	void Add(Light & light);

	std::vector<Light> lights;
	glm::mat4 cameraMatrix;
	glm::mat3 normalMatrix;

	glm::vec4 GetPosition(int i);
	glm::vec4 GetRawPosition(int i);
	glm::vec3 GetDirection(int i);
	glm::vec3 GetRawDirection(int i);
};