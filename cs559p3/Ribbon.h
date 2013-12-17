#pragma once
#include "object.h"
#include <vector>
class Ribbon :
	public Object
{
protected:
	std::vector<glm::vec3> controlPoints;
	std::vector<ILContainer> textures;

public:
	Ribbon(void);
	~Ribbon(void);

	virtual void Initialize();
	virtual void CreateCircularRibbonControlPoints(float radius, int numPoints, int mode = 0);
	
	glm::vec3 GetCameraPosition(float time);

private:
	typedef Object super;
};

