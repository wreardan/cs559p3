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

private:
	typedef Object super;
};

