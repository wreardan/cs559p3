#include "SceneSolarSystem.h"


SceneSolarSystem::SceneSolarSystem(void)
{
	simulationSpeed = 20.0f;
	simulationTime = 0.0f;
	simulationModifier = 1000.0f;
	currentPlanet = 0;
	marsMode = true;
}


SceneSolarSystem::~SceneSolarSystem(void)
{
}

void SceneSolarSystem::Initialize()
{
	//Planet modifiers
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
	
	//Background Stars and other stuff
	super::Initialize();
}

void SceneSolarSystem::Draw()
{
	simulationTime += simulationSpeed * timer.DeltaTime();
	if(simulationTime > simulationModifier) {
		simulationTime = 0.0f;
		currentPlanet = (currentPlanet + 1) % planets.size();
	}
	if(simulationTime < 0.0f) { //we are going backwards in time
		simulationTime = simulationModifier - 1.0f;
		currentPlanet = abs(currentPlanet - 1) % planets.size();
	}

	if(marsMode)
		camera.camPosition = mars.getRibbonPosition(simulationTime / simulationModifier);
	else
		camera.camPosition = planets[currentPlanet].getRibbonPosition(simulationTime / simulationModifier);

	camera.camTarget  = camera.camPosition + 1.0f * camera.forwardDirection;

	super::Draw();

	for(int i = 0; i < (int)planets.size(); i++)
		planets[i].Render(viewMatrix, projectionMatrix, lights);
	mars.Render(viewMatrix, projectionMatrix, lights);
}

void SceneSolarSystem::Update()
{
	super::Update();

	float deltaTime = timer.DeltaTime();

	for(int i = 0; i < (int)planets.size(); i++) 
		planets[i].Update(deltaTime);
	mars.Update(deltaTime);
}


void SceneSolarSystem::Keyboard(unsigned char key, glm::ivec2 point)
{
	switch(key) {
	case 'n':
		ChangeNormalsMode();
		break;
	case 'm':
		marsMode = ! marsMode;
		break;
	case 'f':
	case 'F':
		ChangeWireframeMode();
		break;
	case '+':
		simulationSpeed += 10;
		break;
	case '-':
		simulationSpeed -= 10;
		break;
	default:
		super::Keyboard(key, point);
	}
}



void SceneSolarSystem::ChangeWireframeMode()
{
	for(int i = 0; i < (int)planets.size(); i++) 
		planets[i].wireframeMode = ! planets[i].wireframeMode;
	mars.wireframeMode = ! mars.wireframeMode;
}
void SceneSolarSystem::ChangeNormalsMode()
{
	for(int i = 0; i < (int)planets.size(); i++) 
		planets[i].drawNormals = ! planets[i].drawNormals;
	mars.drawNormals = ! mars.drawNormals;
}