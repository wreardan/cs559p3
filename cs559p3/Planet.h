#pragma once
#include "object.h"
class Planet :
	public Object
{
public:
	Planet(void);
	~Planet(void);

	virtual void Initialize();
	virtual void Update(float deltaTime);

	virtual void LoadTexture(char * fileName);

private:
	typedef Object super;
};

