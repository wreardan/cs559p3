#pragma once
#include "object.h"


#define MERCURY 0
#define VENUS 1
#define EARTH 2
#define JUPITER 3
#define SATURN 4
#define URANUS 5
#define NEPTUNE 6
#define SUN 7



class Planet :
	public Object
{
protected:
	float radius;

	//Each planet will have a position in the world that rotates around the sun
	glm::vec3 planetPosition; // (x, y, z) world position
	glm::mat4 rotationMatrix; //this will be the matrix that rotates the planet, incorporate into world matrix
	glm::vec3 scale;

	//ROTATE PLANET AROUND Y-AXIS BASED ON TIME WILL SIMPLY USE WORLD MATRIX TO DO THE ROTATION
	float currRadiansRotation; //this value will depend on time and is what the planets position will depend on
	float rotationSpeed; //IN SECONDS; this value will determine how fast the planet rotates around the y-axis



	//ROTATE PLANET AROUND THE SUN/ORGIN >> WORLD POSITION BASED ON RADIUS AND TIME
	float radiusOrbit;  //planets radius from the sun, how far to orbit around the sun
	float currRadiansOrbit; //based on time this will position the planet around the sun in a circle
	float orbitSpeed; //IN SECONDS; how fast/long it takes for the planet to make one full orbit around sun

	std::string name;

public:
	Planet(void);
	~Planet(void);

	virtual void Initialize(float radius, float orbitRadius, float orbitSpeed, float rotationSpeed, std::string textureFile);
	virtual void Update(float deltaTime);

	virtual void LoadTexture(std::string fileName);
	
	void calcPosition(float deltaTime); //sets planets position in orbit around sun, based on time and radius
	void calcRotation(float deltaTime); //this calculates the current radians to rotate the planet around the y-axis

private:
	typedef Object super;
};

