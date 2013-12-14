#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "Mesh.h"
#include "glslprogram.h"
#include "ilcontainer.h"
#include "Lights.h"
class Object
{
protected:
	std::vector<Mesh> meshes;
	static GLSLProgram shader;
	static GLSLProgram solidShader;
	
	glm::vec3 pos, dir, up, scale;
	glm::mat4 worldMatrix;

public:
	Object(void);
	~Object(void);

	virtual void Initialize();
	virtual void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, Lights lights);
	virtual void Update(float deltaTime);

	virtual void LoadTexture(std::string fileName);
	virtual void LoadTexture(GLuint textureHandle);

	virtual void SetWorldMatrix(glm::mat4 worldMatrix);

	static void InitializeShader(std::string vertexFilename, std::string fragmentFilename);
	static void FreeShaders();

	bool wireframeMode;
	bool drawNormals;
	bool isStar;
};

