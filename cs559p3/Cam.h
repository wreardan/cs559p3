#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef PI
#define PI 3.141592654
#endif


class Camera {
public:
	//needed for calculating view matrix
	glm::vec3 camPosition;
	glm::vec3 camTarget;
	glm::vec3 camUp;

	//needed to keep track of view direction: forward and right
	glm::vec3 forwardDirection;
	glm::vec3 rightDirection;
	glm::vec3 upDirection;


	//used for looking around, look direction vector based on spherical coords
	float theta, phi;

	float movementScalar;


	//for orbit option
	//variables used for orbiting around mars
	float orbitRadians;
	glm::vec3 positionAroundMars;
	float orbitSpeed;



public:
	Camera();

	//This is the constructor to use:
	Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up);
	~Camera();


	//Calculate position for managed orbit modes
	void Orbit(float deltaTime, float radius, glm::vec3 position);


	//Free fly functions
	void MoveForward();
	void MoveBackward();

	void MoveRight();
	void MoveLeft();

	void TurnRight();
	void TurnLeft();

	void MoveUp();
	void MoveDown();


	//Mouse look, which controls "forward" or "lookAt" direction
	void mouseLook(float dTheta, float dPhi); //positive radians turn left, negative radians turn right
	//void mouseTurnY(float radians); //positive radians look up, negative radians look down

};