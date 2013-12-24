#pragma once

#include "scene.h"
#include "Ribbon.h"

class SceneObjectViewer :
	public Scene
{
protected:
	std::vector<Object *> sceneObjects;
	int currentObject;

	void ChangeWireframeMode();
	void ChangeNormalsMode();

public:
	SceneObjectViewer(void);
	~SceneObjectViewer(void);

	virtual void Initialize();
	virtual void Draw();
	virtual void Keyboard(unsigned char key, glm::ivec2 point);
	
private:
	typedef Scene super;
};

