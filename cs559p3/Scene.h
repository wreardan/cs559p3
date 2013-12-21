#pragma once

#include "Cam.h"
#include "Mesh.h"
#include "Object.h"
#include "Ribbon.h"
#include "Planet.h"
#include "Time.h"
#include "fbo.h"
#include "PostProcessing.h"
#include "Mars.h"

class Scene
{
protected:
//Members
	Planet stars;
	Time timer;
	Camera camera;
	Lights lights;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::ivec2 windowSize;

//Mouse
	glm::ivec2 mousePreviousPosition;
	bool leftButtonDown;
	float perPixelTurnRad;

//Methods
public:
	Scene(void);
	~Scene(void);

	virtual void Initialize();
	virtual void Draw();
	virtual void Update();
	virtual void TakeDown();

	virtual void Resize(glm::ivec2 windowSize);
	virtual void Keyboard(unsigned char key, glm::ivec2 point);
	virtual void Special(int c, glm::ivec2 point);
	virtual void Mouse(int button, int state, glm::ivec2 point);
	virtual void Motion(glm::ivec2 point);


};

