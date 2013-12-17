#pragma once
#include "object.h"
#include <vector>
class Ribbon :
	public Object
{
protected:
	std::vector<glm::vec3> controlPoints;
	std::vector<ILContainer> textures;
	bool changing;
	float time;

public:
	Ribbon(void);
	~Ribbon(void);

	virtual void Initialize();
	virtual void CreateCircularRibbonControlPoints(float radius, int numPoints, int mode = 0, float deltaTime = 0.0f);
	virtual void Update(float deltaTime);
	
	glm::vec3 GetCameraPosition(float time);

private:
	typedef Object super;
};

