#pragma once
#include "planet.h"
class Mars :
	public Planet
{
public:
	Mars(void);
	~Mars(void);

	virtual void Initialize(float radius, float orbitRadius, float orbitSpeed, float rotationSpeed, std::string textureFile);
};

