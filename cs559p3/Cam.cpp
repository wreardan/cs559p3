
#include "Cam.h"

#include <iostream>

using namespace glm;

float turnRadians = (float)(3.14159 / 16); //how much to turn/rotate by, in radians.

Camera::Camera() {
		camPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		camTarget = glm::vec3(0.0f, 0.0f, -1.0f);
		camUp = glm::vec3(0.0f, 1.0f, 0.0f);

		forwardDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		rightDirection = glm::vec3(1.0f, 0.0f, 0.0f);
		upDirection = glm::vec3(0.0f, 1.0f, 0.0f);

		
}

Camera::Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up) {
	camPosition = pos;
	camTarget = target;
	camUp = up;
	//add params for these too
	forwardDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	rightDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	upDirection = glm::vec3(0.0f, 1.0f, 0.0f);

	movementScalar = 0.5f;

	//init to (0, 0, -1)
	theta = (float)(PI / 2);
	phi = (float)(3.0f * PI / 2);


	//orbit stuff
	orbitRadians = 0.0f;
	positionAroundMars = vec3(0.0f, 0.0f, 0.0f);
	orbitSpeed = float(2.0f * PI / 17.0f);

}

Camera::~Camera() {
}

void Camera::MoveForward() {
	camPosition = camPosition + ( movementScalar * forwardDirection );
	camTarget = camPosition + forwardDirection;
}
void Camera::MoveBackward() {
	camPosition = camPosition - ( movementScalar * forwardDirection );
	camTarget = camPosition + forwardDirection;
}

void Camera::MoveRight() {
	camPosition = camPosition + ( movementScalar * rightDirection );
	camTarget = camPosition + forwardDirection;
}

void Camera::MoveLeft() {
	camPosition = camPosition - ( movementScalar * rightDirection );
	camTarget = camPosition + forwardDirection;
}

void Camera::TurnRight() {
	glm::mat4 rotationMatrix(1.0f);
	rotationMatrix[0].x = cosf(-turnRadians);
	rotationMatrix[0].z = -sinf(-turnRadians);
	rotationMatrix[2].x = sinf(-turnRadians);
	rotationMatrix[2].z = cosf(-turnRadians);

	glm::vec4 temp(forwardDirection, 1.0f);
	temp = rotationMatrix * temp;
	forwardDirection.x = temp.x;
	forwardDirection.y = temp.y;
	forwardDirection.z = temp.z;


	glm::vec4 temp2(rightDirection, 1.0f);
	temp2 = rotationMatrix * temp2;
	rightDirection.x = temp2.x;
	rightDirection.y = temp2.y;
	rightDirection.z = temp2.z;

	camTarget = camPosition + forwardDirection;
}


void Camera::TurnLeft() {
	glm::mat4 rotationMatrix(1.0f);
	rotationMatrix[0].x = cosf(turnRadians);
	rotationMatrix[0].z = -sinf(turnRadians);
	rotationMatrix[2].x = sinf(turnRadians);
	rotationMatrix[2].z = cosf(turnRadians);

	glm::vec4 temp(forwardDirection, 1.0f);
	temp = rotationMatrix * temp;
	forwardDirection.x = temp.x;
	forwardDirection.y = temp.y;
	forwardDirection.z = temp.z;


	glm::vec4 temp2(rightDirection, 1.0f);
	temp2 = rotationMatrix * temp2;
	rightDirection.x = temp2.x;
	rightDirection.y = temp2.y;
	rightDirection.z = temp2.z;

	camTarget = camPosition + forwardDirection;
}

void Camera::MoveUp() {
	camPosition = camPosition + (1.0f * glm::vec3(0.0f, 1.0f, 0.0f) );
	camTarget = camPosition + forwardDirection;
}

void Camera::MoveDown() {
	camPosition = camPosition + (1.0f * glm::vec3(0.0f, -1.0f, 0.0f) );
	camTarget = camPosition + forwardDirection;
}



void Camera::mouseLook(float dTheta, float dPhi) {
	theta += dTheta;
	phi += dPhi;
	if(theta <= 0.3f) { theta = 0.3f; }
	if(theta >= (PI - 0.3f) ) { theta = (float)(PI - 0.3f); }


	float x, y, z;

	x = 1.0f * sinf(theta) * cosf(phi);
	y = 1.0f * cosf(theta);
	z = 1.0f * sinf(theta) * sinf(phi);

	//have new look direction vector
	//get look at target one unit in front of cam position
	forwardDirection = glm::normalize( glm::vec3(x, y, z) );
	camTarget = camPosition + forwardDirection;


	rightDirection = glm::normalize( glm::cross( forwardDirection, camUp ) );
	upDirection = glm::normalize( glm::cross( rightDirection, forwardDirection ) );


}

void Camera::Orbit(float deltaTime, float radius, glm::vec3 position)
{

	float deltaRadians = orbitSpeed * deltaTime;
	orbitRadians -= deltaRadians;
	if(orbitRadians <= -1.0f * 2.0f * PI) { orbitRadians = 0.0f; }

	//get current direction around mars
	float x, z;
	x = 1.0f * cosf(orbitRadians);
	z = 1.0f * sinf(orbitRadians);
	vec3 direction(x, 0.0f, z);

	//get orbit radius from mars
	float radiusFromMarsPosition = radius * 1.5f; //magnitude

	//get magnitude of direction to go from mars position
	direction = direction * radiusFromMarsPosition;

	positionAroundMars = position + direction;
	camPosition = positionAroundMars;
	camUp = normalize(direction);
	camTarget = cross(vec3(0.0f, 1.0f, 0.0f), direction );
	forwardDirection = camTarget;
	rightDirection = vec3(0.0f, -1.0f, 0.0f);
}
