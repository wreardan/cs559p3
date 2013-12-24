#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

Scene::Scene(void)
{
	mousePreviousPosition = ivec2(0,  0);
	leftButtonDown = false;
	perPixelTurnRad = 0.005f;
}


Scene::~Scene(void)
{
}

void Scene::Initialize()
{
	Object::InitializeShader("Shaders/planet.vert", "Shaders/planet.frag");

	//STARS
	stars.Initialize(1500.0f, 0.0f, 0.0f, 0.0f, "stars6.jpg");
	stars.isStar = true;
	stars.Update(0.0f);
	
	projectionMatrix = perspective(55.0f, 1024.0f/768.0f, 0.1f, 10000.0f);
	camera = Camera(vec3(0.0f, 30.0f, 250.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	lights.Add(Light(vec3(0.0f, 100.0f, 0.1f)));
	//lights.Add(Light(vec3(-0.5f, -1.0f, 1.0f)));

	//Timer initialization
	timer.Reset();
}

void Scene::Draw()
{
	//Timer tick, update deltaTime
	timer.Tick();

	//Setup camera and lights
	viewMatrix = lookAt(camera.camPosition, camera.camTarget, camera.camUp);
	lights.cameraMatrix = viewMatrix;

	//Draw background stars
	glDisable(GL_CULL_FACE);
	stars.Render(viewMatrix, projectionMatrix, lights);
	glEnable(GL_CULL_FACE);
}

void Scene::Update()
{
}

void Scene::TakeDown()
{

}

void Scene::Resize(glm::ivec2 windowSize)
{
	this->windowSize = windowSize;

	//Update perspective
	float aspect = (float)windowSize.x / (float)windowSize.y;
	projectionMatrix = perspective(55.0f, aspect, 0.1f, 10000.0f);

	glViewport(0, 0, windowSize.x, windowSize.y);
}

void Scene::Keyboard(unsigned char key, glm::ivec2 point)
{
	switch(key) {
	case 'w':
	case 'W':
		camera.MoveForward();
		break;
	case 's':
	case 'S':
		camera.MoveBackward();
		break;
	case 'a':
	case 'A':
		camera.MoveLeft();
		break;
	case 'd':
	case 'D':
		camera.MoveRight();
		break;
	case 'q':
	case 'Q':
		camera.MoveUp();
		break;
	case 'e':
	case 'E':
		camera.MoveDown();
		break;
	case 'p':
	case 'P':
		if(timer.mStopped) {
			timer.Start();
		}else {
			timer.Stop();
		}
		break;
	case 27:   //escape
		glutLeaveMainLoop();
		break;
	default:
		std::cout << "unhandled keyboard key: " << key << std::endl;
		break;
	}
}

void Scene::Special(int c, glm::ivec2 point)
{
	switch (c)
	{
	case GLUT_KEY_UP:
		camera.MoveForward();
		break;

	case GLUT_KEY_DOWN:
		camera.MoveBackward();
		break;

	case GLUT_KEY_LEFT:
		camera.MoveLeft();
		break;

	case GLUT_KEY_RIGHT:
		camera.MoveRight();
		break;

	default:
		std::cout << "unhandled special key: " << c << std::endl;
	}
}


void Scene::Mouse(int button, int state, glm::ivec2 point)
{
	if(button == GLUT_RIGHT_BUTTON) {
		//no functionality for right button
	}else if(button == GLUT_LEFT_BUTTON) {
		if(state == GLUT_DOWN) {
			leftButtonDown = true;
			mousePreviousPosition.x = point.x;
			mousePreviousPosition.y = point.y;
		}else { 
			leftButtonDown = false;
			mousePreviousPosition.x = 0;
			mousePreviousPosition.y = 0;
		}
	}
}

void Scene::Motion(glm::ivec2 point)
{
	//only look around if the left button on the mouse is clicked
	if(leftButtonDown) {
		//dx and dy are values of how much to look based on how many pixel we have moved and is in terms of radians
		float dx = (float) ((point.x - mousePreviousPosition.x) * perPixelTurnRad ); 

		float dy = (float) ((point.y - mousePreviousPosition.y) * perPixelTurnRad ); 

		//update camera vectors: forward, right and up 
		camera.mouseLook(dy, dx);
		

		mousePreviousPosition.x = point.x;
		mousePreviousPosition.y = point.y;
	}
}
