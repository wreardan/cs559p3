/*
	CS 559 Project 3
	Fall 2013
	(c) 2013 Wesley Reardan and Dustin Greene

 */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <IL/il.h>

using namespace std;

#include "Window.h"
#include "Cam.h"
Window window;

Camera camera;

//TESTING BEGIN
#include "Mesh.h"
#include "Object.h"
#include "Ribbon.h"
Mesh mesh;
Object object;
Ribbon ribbon;

//Scene Code, seperate this into Scene Class eventually
#include "glslprogram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assert.h>
using namespace glm;

GLSLProgram program;
glm::mat4 ProjectionMatrix;
Lights lights;

void SceneDraw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 ViewMatrix = lookAt(camera.camPosition, camera.camTarget, camera.camUp);
	lights.cameraMatrix = ViewMatrix;
	mat4 modelView = translate(ViewMatrix, vec3(0.0f, -1.5f, 0.0f));
	//object.Render(modelView, ProjectionMatrix, lights);
	ribbon.Render(modelView, ProjectionMatrix, lights);

	/*program.use();
	mat4 modelView = scale(ViewMatrix, vec3(3.0f));
	program.setUniform("MVP", ProjectionMatrix * modelView);
	program.setUniform("SolidColor", vec4(1.0f, 0.0f, 0.0f, 1.0f));

	mesh.Draw();

	program.setUniform("SolidColor", vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mesh.DrawNormals();*/

}
void SceneInit()
{
	//Object::InitializeShader("Shaders/Textured.vert", "Shaders/Textured.frag");
	Object::InitializeShader("Shaders/Phong.vert", "Shaders/Phong.frag");

	//Compile Shader
	/*assert(program.compileShaderFromFile("Shaders/solid.vert", GLSLShader::VERTEX));
	assert(program.compileShaderFromFile("Shaders/solid.frag", GLSLShader::FRAGMENT));
	assert(program.link());
	assert(program.validate());*/
	ProjectionMatrix = perspective(45.0f, (float)window.size.x / window.size.y, 0.1f, 100.0f);

	//object.Initialize();
	ribbon.Initialize();
	//Create mesh
	/*mesh.Initialize(64, 64);*/

	camera = Camera(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	lights.Add(Light(vec3(1.0f, 1.0f, 1.0f)));
	lights.Add(Light(vec3(-0.5f, -1.0f, 1.0f)));
}
//SCENE END


//TESTING END

void KeyboardFunc(unsigned char key, int x, int y) {
	switch(key) {
	case 'w':
	case 'W':
		mesh.wireframeMode = !mesh.wireframeMode;
		break;
	case 'q':
		camera.MoveUp();
		break;
	case 'e':
		camera.MoveDown();
		break;
	case 27:   //escape
		glutLeaveMainLoop();
		break;
	}

}



//Mouse
ivec2 mousePreviousPosition(0,  0);
bool leftButtonDown = false;
float perPixelTurnRad = 0.005f;

void mouse(int button, int state, int x, int y) {
	if(button == GLUT_RIGHT_BUTTON) {
		//no functionality for right button
	}else if(button == GLUT_LEFT_BUTTON) {
		if(state == GLUT_DOWN) {
			leftButtonDown = true;
			mousePreviousPosition.x = x;
			mousePreviousPosition.y = y;
		}else { 
			leftButtonDown = false;
			mousePreviousPosition.x = 0;
			mousePreviousPosition.y = 0;
		}
	}
}
void motion(int x, int y) {
	//only look around if the left button on the mouse is clicked
	if(leftButtonDown) {
		//dx and dy are values of how much to look based on how many pixel we have moved and is in terms of radians
		float dx = (float) ((x - mousePreviousPosition.x) * perPixelTurnRad ); 

		float dy = (float) ((y - mousePreviousPosition.y) * perPixelTurnRad ); 

		//update camera vectors: forward, right and up 
		camera.mouseLook(dy, dx);
		

		mousePreviousPosition.x = x;
		mousePreviousPosition.y = y;
	}
}

void SpecialFunc(int c, int x, int y)
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
		cout << "unhandled special key: " << c << endl;
	}
}



void DisplayFunc()
{
	//Draw Scene
	SceneDraw();

	glutPostRedisplay();
	glutSwapBuffers();
}

bool Initialize(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitWindowSize(window.size.x, window.size.y);
	glutInitWindowPosition(window.size.x/4, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	window.handle = glutCreateWindow("CS 559 Project 3 - Wesley Reardan & Dustin Greene");
	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);

	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(SpecialFunc);

	if(glewInit() != GLEW_OK) {
		cerr << "Glew failed to initialize" << endl;
		return false;
	}

	ilInit();

	//Intialize Scene
	SceneInit();

	return true;
}



int main(int argc, char* argv[])
{
	if(!Initialize(argc, argv))
		return 1;
	glutMainLoop();
	return 0;
}
