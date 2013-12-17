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
#include "Planet.h"
#include "Time.h"
#include "fbo.h"
#include "PostProcessing.h"
#include "Mars.h"

Time timer;

Mesh mesh;
Object object;
Ribbon ribbon;
vector<Planet> planets;
Planet stars;
Mars mars;

PostProcessing postProcess;

//Scene Code, seperate this into Scene Class eventually
#include "glslprogram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assert.h>
using namespace glm;

GLSLProgram postProcessShader;
glm::mat4 ProjectionMatrix;
Lights lights;

void ChangeWireframeMode()
{
	for(int i = 0; i < planets.size(); i++) 
		planets[i].wireframeMode = ! planets[i].wireframeMode;
	ribbon.wireframeMode = ! ribbon.wireframeMode;
	
}


//Update function for timer
void Update(float deltaTime) {
	//update planets in solar system
	for(int i = 0; i < planets.size(); i++) 
		planets[i].Update(deltaTime);
	ribbon.Update(deltaTime);
	mars.Update(deltaTime);
}


void SceneDraw()
{

	//Timer tick, update deltaTime
	timer.Tick();
	Update(timer.DeltaTime());

	//OpenGL calls
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//mat4 ViewMatrix = lookAt(camera.camPosition, camera.camTarget, camera.camUp);
	//vec3 camPos = planets[SATURN].getRibbonPosition(timer.TotalTime() / 10.0f);
	vec3 camPos = mars.getRibbonPosition(timer.TotalTime() / 10.0f);
	vec3 camTarget = camPos + 1.0f * camera.forwardDirection;
	mat4 ViewMatrix = lookAt(camPos, camTarget, camera.camUp);
	lights.cameraMatrix = ViewMatrix;

	mat4 modelView = translate(ViewMatrix, vec3(0.0f, -1.5f, 0.0f));
	//object.Render(modelView, ProjectionMatrix, lights);

	//ribbon.isStar = true;
	//ribbon.Render(modelView, ProjectionMatrix, lights);


	for(int i = 0; i < planets.size(); i++)
		planets[i].Render(ViewMatrix, ProjectionMatrix, lights);
	mars.Render(ViewMatrix, ProjectionMatrix, lights);

	/*program.use();
	mat4 modelView = scale(ViewMatrix, vec3(3.0f));
	program.setUniform("MVP", ProjectionMatrix * modelView);
	program.setUniform("SolidColor", vec4(1.0f, 0.0f, 0.0f, 1.0f));

	mesh.Draw();

	program.setUniform("SolidColor", vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mesh.DrawNormals();*/
	
	glDisable(GL_CULL_FACE);
	stars.Render(ViewMatrix, ProjectionMatrix, lights);

	glDisable(GL_DEPTH_TEST);
}
void SceneInit()
{
	//Object::InitializeShader("Shaders/Textured.vert", "Shaders/Textured.frag");
	Object::InitializeShader("Shaders/planet.vert", "Shaders/planet.frag");

	//Compile Shader

	//object.Initialize();
	ribbon.Initialize();
	
	//Planets
	float distanceModifier = 2.0f;
	float sizeModifier = 4.0f;
	float orbitMod = 10.0f;
	float sunSize = 100.0f;
	//Initialize planets
	planets.resize(8);
	//MERCURY
	planets[MERCURY].Initialize(float(3.83f/sizeModifier * 2), float( 38.7/distanceModifier + sunSize), (float)( 2.0f * PI / (24.0f/orbitMod) ), (float)( 2.0f * PI / 10.0f ), "mercuryHiRes.jpg");
	//VENUS
	planets[VENUS].Initialize(float(9.49f/sizeModifier * 2), float(90.3/distanceModifier + sunSize), (float)( 2.0f * PI / (61.5f/orbitMod )), (float)( 2.0f * PI / 15.0f ), "venusHiRes.jpg");
	//EARTH
	planets[EARTH].Initialize(float(10.0f/sizeModifier * 2), float(200/distanceModifier + sunSize), (float)( 2.0f * PI / (100.0f/orbitMod )), (float)( 2.0f * PI / 20.0f ), "earthHiRes.jpg");
	//JUPITER
	planets[JUPITER].Initialize(float(112.10f/sizeModifier), float(520.0/distanceModifier + sunSize), (float)( 2.0f * PI / (111.0f/orbitMod )), (float)( 2.0f * PI / 25.0f ), "jupiter.jpg");
	//SATURN
	planets[SATURN].Initialize(float(94.20f/sizeModifier), float(1208.0/distanceModifier + sunSize), (float)( 2.0f * PI / (290.0f/orbitMod)), (float)( 2.0f * PI / 30.0f ), "saturnHiRes.jpg");
	//URANUS
	planets[URANUS].Initialize(float(48.18f/sizeModifier), float(1920.0/distanceModifier + sunSize), (float)( 2.0f * PI / (837.0f/orbitMod )), (float)( 2.0f * PI / 35.0f ), "uranus.jpg");
	//NEPTUNE
	planets[NEPTUNE].Initialize(float(38.8f/sizeModifier), float(3005.0/distanceModifier + sunSize), (float)( 2.0f * PI / (1637.0f/orbitMod )), (float)( 2.0f * PI / 40.0f ), "neptune.jpg");
	//SUN
	planets[SUN].Initialize(50.0f, 0.0f, 0.0f, (float)( 2.0f * PI / 10.0f ), "sun4.jpg");
	planets[SUN].isStar = true;
	//MARS
	mars.Initialize(130.0f/sizeModifier, float(350.0/distanceModifier + sunSize), (float)( 2.0f * PI / (111.0f/orbitMod )) , 2.0f * float(PI) / 13.0f, "mars.jpg");
	//STARS
	stars.Initialize(1500.0f, 0.0f, 0.0f, 0.0f, "stars6.jpg");
	stars.isStar = true;
	stars.Update(0.0f);

	camera = Camera(vec3(0.0f, 30.0f, 250.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	lights.Add(Light(vec3(0.0f, 100.0f, 0.1f)));
	//lights.Add(Light(vec3(-0.5f, -1.0f, 1.0f)));

	//Timer initialization
	timer.Reset();

}
//SCENE END

void DisplayFunc()
{
	float time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;

	postProcess.Bind();

	//Draw Scene
	SceneDraw();

	postProcess.UnBind();

	postProcess.Draw(time);

	glutPostRedisplay();
	glutSwapBuffers();
}


//TESTING END

void ResizeFunc(int width, int height)
{
	window.size.x = 1024;
	window.size.y = 768;

	postProcess.ReSize(window.size);

	ProjectionMatrix = perspective(45.0f, (float)window.size.x / window.size.y, 0.1f, 4000.0f);

	glViewport(0, 0, width, height);
}


void KeyboardFunc(unsigned char key, int x, int y) {
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
	case 'n':
		ribbon.drawNormals = ! ribbon.drawNormals;
		break;
	case 'm':
		ribbon.wireframeMode = ! ribbon.wireframeMode;
		break;
	case 'p':
		if(timer.mStopped) {
			timer.Start();
		}else {
			timer.Stop();
		}
		break;
	case 'f':
	case 'F':
		ChangeWireframeMode();
		break;
	case 'o':
	case 'O':
		postProcess.ChangeEffect();
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

	
	//Initialization for Framebuffer stuff
	postProcess.Initialize(window.size);

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
