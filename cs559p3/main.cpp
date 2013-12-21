/*
	CS 559 Project 3
	Fall 2013
	(c) 2013 Wesley Reardan and Dustin Greene

 */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <IL/il.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

#include "Window.h"
#include "Cam.h"
#include "SceneSolarSystem.h"
#include "PostProcessing.h"
Window window;
SceneSolarSystem scene;
PostProcessing postProcess;

void DisplayFunc()
{
	//Bind post processing framebuffer
	postProcess.Bind();

	//Update scene
	scene.Update();

	//OpenGL Enables
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Draw Scene
	scene.Draw();

	//OpenGL Disable
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	//Unbind framebuffer and draw with filter
	postProcess.UnBind();
	postProcess.Draw(0.0f);

	//post to screen
	glutPostRedisplay();
	glutSwapBuffers();
}


//TESTING END

void ResizeFunc(int width, int height)
{
	scene.Resize(ivec2(width, height));
}


void KeyboardFunc(unsigned char key, int x, int y) {
	switch(key)
	{
	case 'o':
	case 'O':
		postProcess.ChangeEffect();
		break;
	}

	scene.Keyboard(key, ivec2(x, y));

}



//Mouse
void mouse(int button, int state, int x, int y) {
	scene.Mouse(button, state, ivec2(x, y));
}
void motion(int x, int y) {
	scene.Motion(ivec2(x, y));
}
void SpecialFunc(int c, int x, int y)
{
	scene.Special(c, ivec2(x, y));
}
void Close()
{

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
	glutReshapeFunc(ResizeFunc);
	glutCloseFunc(Close);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	if(glewInit() != GLEW_OK) {
		cerr << "Glew failed to initialize" << endl;
		return false;
	}

	ilInit();	//Initialize DevIL

	//Intialize Scene
	//SceneInit();
	scene.Resize(window.size);
	scene.Initialize();
	//Initialization for Framebuffer stuff
	postProcess.Initialize(window.size);

	return true;
}



int main(int argc, char* argv[])
{
	if(!Initialize(argc, argv))
		return 1;
	glutMainLoop();
	return 0;
}
