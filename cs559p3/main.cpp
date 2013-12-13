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
#include "fbo.h"
Window window;

Camera camera;

//TESTING BEGIN
#include "Mesh.h"
#include "Object.h"
#include "Ribbon.h"
#include "Planet.h"
#include "Time.h"

Time timer;

Mesh mesh;
Object object;
Ribbon ribbon;
vector<Planet> planets;
Planet stars;
FrameBufferObject fbo;
Object framebufferDrawObject;
GLuint framebufferSquareHandle, framebufferSquareVAO;

//Scene Code, seperate this into Scene Class eventually
#include "glslprogram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assert.h>
using namespace glm;

GLSLProgram program, postProcessShader;
glm::mat4 ProjectionMatrix;
Lights lights;


//Update function for timer
void Update(float deltaTime) {
	//update planets in solar system
	for(int i = 0; i < planets.size(); i++) 
		planets[i].Update(deltaTime);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 ViewMatrix = lookAt(camera.camPosition, camera.camTarget, camera.camUp);
	lights.cameraMatrix = ViewMatrix;
	//mat4 modelView = translate(ViewMatrix, vec3(0.0f, -1.5f, 0.0f));
	//object.Render(modelView, ProjectionMatrix, lights);
	//ribbon.Render(modelView, ProjectionMatrix, lights);
	for(int i = 0; i < planets.size(); i++)
		planets[i].Render(ViewMatrix, ProjectionMatrix, lights);


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
	/*assert(program.compileShaderFromFile("Shaders/solid.vert", GLSLShader::VERTEX));
	assert(program.compileShaderFromFile("Shaders/solid.frag", GLSLShader::FRAGMENT));
	assert(program.link());
	assert(program.validate());*/
	ProjectionMatrix = perspective(45.0f, (float)window.size.x / window.size.y, 0.1f, 2000.0f);

	//object.Initialize();
	ribbon.Initialize();
/*
	planets.resize(7);
	
	planets[0].Initialize(0.766f, 10.0f, (float)( 105.0f ), (float)( 2.0f * PI / 10.0f ), "Textures/mercuryHiRes.jpg");
	planets[1].Initialize(1.898f, 30.0f, (float)( 110.33f ), (float)( 2.0f * PI / 10.0f ), "Textures/mercuryHiRes.jpg");
	planets[2].Initialize(0.5f, 40.0f, (float)( 128.57f ), (float)( 2.0f * PI / 10.0f ), "Textures/mercuryHiRes.jpg");
	planets[3].Initialize(11.21f, 45.0f, (float)( .0f ), (float)( 2.0f * PI / 10.0f ), "Textures/mercuryHiRes.jpg");
	planets[4].Initialize(9.42f, 30.0f, (float)( 2.0f * PI / 20.0f ), (float)( 2.0f * PI / 10.0f ), "Textures/mercuryHiRes.jpg");
	planets[5].Initialize(4.818f, 30.0f, (float)( 2.0f * PI / 20.0f ), (float)( 2.0f * PI / 10.0f ), "Textures/mercuryHiRes.jpg");
	planets[6].Initialize(3.88f, 30.0f, (float)( 2.0f * PI / 20.0f ), (float)( 2.0f * PI / 10.0f ), "Textures/mercuryHiRes.jpg");
	*/
	float distanceModifier = 7.0f;
	float sizeModifier = 4.0f;
	float orbitMod = 10.0f;
	float sunSize = 100.0f;
	//Planets
	//Initialize planets
	planets.resize(8);

	//MERCURY
	planets[MERCURY].Initialize(float(3.83f/sizeModifier * 2), float( 38.7/distanceModifier + sunSize), (float)( 2.0f * PI / (24.0f/orbitMod) ), (float)( 2.0f * PI / 10.0f ), "mercuryHiRes.jpg");
	//VENUS
	planets[VENUS].Initialize(float(9.49f/sizeModifier * 2), float(72.3/distanceModifier + sunSize), (float)( 2.0f * PI / (61.5f/orbitMod )), (float)( 2.0f * PI / 15.0f ), "venusHiRes.jpg");
	//EARTH
	planets[EARTH].Initialize(float(10.0f/sizeModifier * 2), float(200/distanceModifier + sunSize), (float)( 2.0f * PI / (100.0f/orbitMod )), (float)( 2.0f * PI / 20.0f ), "earthHiRes.jpg");
	//JUPITER
	planets[JUPITER].Initialize(float(112.10f/sizeModifier), float(520.0/distanceModifier + sunSize), (float)( 2.0f * PI / (111.0f/orbitMod )), (float)( 2.0f * PI / 25.0f ), "jupiter.jpg");
	//SATURN
	planets[SATURN].Initialize(float(94.20f/sizeModifier), float(958.0/distanceModifier + sunSize), (float)( 2.0f * PI / (290.0f/orbitMod)), (float)( 2.0f * PI / 30.0f ), "saturnHiRes.jpg");
	//URANUS
	planets[URANUS].Initialize(float(48.18f/sizeModifier), float(1920.0/distanceModifier + sunSize), (float)( 2.0f * PI / (837.0f/orbitMod )), (float)( 2.0f * PI / 35.0f ), "uranus.jpg");
	//NEPTUNE
	planets[NEPTUNE].Initialize(float(38.8f/sizeModifier), float(3005.0/distanceModifier + sunSize), (float)( 2.0f * PI / (1637.0f/orbitMod )), (float)( 2.0f * PI / 40.0f ), "neptune.jpg");
	//SUN
	planets[SUN].Initialize(50.0f, 0.0f, 0.0f, (float)( 2.0f * PI / 10.0f ), "sun4.jpg");
	planets[SUN].isStar = true;
	//STARS
	stars.Initialize(1500.0f, 0.0f, 0.0f, 0.0f, "stars6.jpg");
	stars.isStar = true;
	stars.Update(0.0f);

	/*for(int i = 0; i < planets.size(); i++) {
		planet.Initialize();
		planet.LoadTexture("Textures/earthHiRes.jpg");
	}*/

	camera = Camera(vec3(0.0f, 0.0f, 180.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	lights.Add(Light(vec3(0.1f, 0.1f, 0.1f)));
	//lights.Add(Light(vec3(-0.5f, -1.0f, 1.0f)));

	//Timer initialization
	timer.Reset();

}
//SCENE END

void UseFramebufferToDrawSomething()
{
	float time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;

	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, window.size.x, window.size.y);
	mat4 ViewMatrix = lookAt(vec3(0, 0, 5.5), vec3(0, 0, 0), vec3(0, 1, 0));
	
	postProcessShader.use();
	vec2 windowSize = vec2(window.size.x, window.size.y);
	postProcessShader.setUniform("size", windowSize);
	postProcessShader.setUniform("ColorMap", (int)0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo.texture_handles[0]);
	
	glBindVertexArray(framebufferSquareVAO);
	glDrawArrays(GL_QUADS, 0, 4);
	glBindVertexArray(0);

	postProcessShader.unuse();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}


void DisplayFunc()
{
	fbo.Bind();
	//Draw Scene
	SceneDraw();

	fbo.Unbind();

	UseFramebufferToDrawSomething();

	glutPostRedisplay();
	glutSwapBuffers();
}


//TESTING END

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

	
	//Initialization for Framebuffer stuff

	fbo.Initialize(window.size);

	mesh.Initialize(64, 64);
	mesh.CreatePlanarMesh(64, 64);
	mesh.CreateIndices();
	mesh.CalculateNormals();
	mesh.CreateTextureCoords();
	//mesh.textures.push_back(new ILContainer());
	//mesh.textures[0]->il_texture_handle = fbo.texture_handles[0];
	//mesh.textures[0]->il_handle = 1;
	framebufferDrawObject.Initialize();
	framebufferDrawObject.LoadTexture(fbo.texture_handles[0]);
	
	//Compile TextureOnly Shader
	if(!postProcessShader.compileShaderFromFile("Shaders/PostProcess.vert", GLSLShader::VERTEX))
		cerr << "Vertex Shader failed to compile: " << postProcessShader.log() << endl;
	if(!postProcessShader.compileShaderFromFile("Shaders/PostProcess.frag", GLSLShader::FRAGMENT))
		cerr << "Fragment Shader failed to compile: " << postProcessShader.log() << endl;
	if(!postProcessShader.link())
		cerr << "Shader Program failed to link: " << postProcessShader.log() << endl;
	if(!postProcessShader.validate())
		cerr << "Shader Program did not validate: " << postProcessShader.log() << endl;

	
	glGenVertexArrays(1, &framebufferSquareVAO);
	glBindVertexArray(framebufferSquareVAO);

	float square[] = {-1.0f,  1.0f,  // 0, Top Left
          -1.0f, -1.0f,  // 1, Bottom Left
           1.0f, -1.0f,  // 2, Bottom Right
           1.0f,  1.0f,  // 3, Top Right
	};
	glGenBuffers(1, &framebufferSquareHandle);
	glBindBuffer(GL_ARRAY_BUFFER, framebufferSquareHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2(1.0f)) * 4, square, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2(1.0f)), 0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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
