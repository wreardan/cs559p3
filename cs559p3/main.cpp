/*
	CS 559 Project 3
	Fall 2013
	(c) 2013 Wesley Reardan and Dustin Greene

 */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

#include "Window.h"
Window window;

//TESTING BEGIN
#include "Mesh.h"
Mesh mesh;

//Scene Code, seperate this into Scene Class eventually
#include "glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assert.h>
using namespace glm;

GLSLProgram program;
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
void SceneDraw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program.use();
	mat4 modelView = scale(ViewMatrix, vec3(3.0f));
	program.setUniform("MVP", ProjectionMatrix * modelView);
	program.setUniform("SolidColor", vec4(1.0f, 0.0f, 0.0f, 1.0f));

	mesh.Draw();

	program.setUniform("SolidColor", vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mesh.DrawNormals();

}
void SceneInit()
{
	//Compile Shader
	assert(program.compileShaderFromFile("Shaders/solid.vert", GLSLShader::VERTEX));
	assert(program.compileShaderFromFile("Shaders/solid.frag", GLSLShader::FRAGMENT));
	assert(program.link());
	assert(program.validate());
	ViewMatrix = lookAt(vec3(5.0f), vec3(0.0f), vec3(0,1,0));
	ProjectionMatrix = perspective(45.0f, (float)window.size.x / window.size.y, 0.1f, 100.0f);

	//Create mesh
	mesh.Initialize(64, 64);
}
//SCENE END


//TESTING END

void KeyboardFunc(unsigned char key, int x, int y) {
	switch(key) {
	case 'w':
	case 'W':
		mesh.wireframeMode = !mesh.wireframeMode;
		break;
	case 27:   //escape
		glutLeaveMainLoop();
		break;
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

	if(glewInit() != GLEW_OK) {
		cerr << "Glew failed to initialize" << endl;
		return false;
	}

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
