#pragma once
#include "planet.h"
class Mars :
	public Planet
{
public:
	Mars(void);
	~Mars(void);

	virtual void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, Lights lights);
	virtual void Initialize(float radius, float orbitRadius, float orbitSpeed, float rotationSpeed, std::string textureFile);
	virtual void Update(float deltaTime);

private:
	typedef Planet super;
};

