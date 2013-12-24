#include "SceneObjectViewer.h"


SceneObjectViewer::SceneObjectViewer(void)
{
	currentObject = 0;
}


SceneObjectViewer::~SceneObjectViewer(void)
{
}

void SceneObjectViewer::Initialize()
{
	Ribbon * ribbon = new Ribbon();
	ribbon->Initialize();
	sceneObjects.push_back(ribbon);

	super::Initialize();

	camera.camPosition = vec3(0.0f, 1.0f, 10.0f);
}

void SceneObjectViewer::Draw()
{
	super::Draw();

	if(currentObject < sceneObjects.size())
		sceneObjects[currentObject]->Render(viewMatrix, projectionMatrix, lights);

}

void SceneObjectViewer::ChangeWireframeMode()
{
	for(int i = 0; i < sceneObjects.size(); i++) {
		sceneObjects[i]->wireframeMode = ! sceneObjects[i]->wireframeMode;
	}

}

void SceneObjectViewer::ChangeNormalsMode()
{
	for(int i = 0; i < sceneObjects.size(); i++) {
		sceneObjects[i]->drawNormals = ! sceneObjects[i]->drawNormals;
	}

}


void SceneObjectViewer::Keyboard(unsigned char key, glm::ivec2 point)
{
	switch(key)
	{
	case 'f':
		ChangeWireframeMode();
		break;
	case 'n':
		ChangeNormalsMode();
		break;
	default:
		super::Keyboard(key, point);
	}

}


