#pragma once
#include "scene.h"
class SceneSolarSystem :
	public Scene
{
protected:
	std::vector<Planet> planets;
	Mars mars;
	int currentPlanet;
	bool marsMode;
	
	float simulationSpeed;
	float simulationTime;
	float simulationModifier;

	void ChangeWireframeMode();
	void ChangeNormalsMode();

public:
	SceneSolarSystem(void);
	~SceneSolarSystem(void);

	virtual void Initialize();
	virtual void Draw();
	virtual void Update();
	virtual void Keyboard(unsigned char key, glm::ivec2 point);

private:
	typedef Scene super;
};

