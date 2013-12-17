#pragma once

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "ilcontainer.h"

#include <GL/glew.h>
#include <cuda.h>
#include <cuda_gl_interop.h>
#include <glm/glm.hpp>
#include <vector>

#include "glslprogram.h"

#ifndef GL_BAD_VALUE
#define GL_BAD_VALUE ((unsigned)-1)
#endif

class Mesh
{
protected:
//Mesh Properties
	int width, height;
//Vertex Attributes
	GLuint vao;
//Vertex Positions, Normals, TextureCoords
	GLuint vboPosition;
	cudaGraphicsResource* resPosition;
	GLuint vboNormals;
	cudaGraphicsResource* resNormals;
	GLuint vboTextureCoords;
	cudaGraphicsResource* resTextureCoords;
//Vertex Indices
	GLuint vboIndices;
	cudaGraphicsResource* resIndices;
	GLuint numIndices;
//Wireframe Indices (uses same Vertices)
	GLuint vboWireframeIndices;
	cudaGraphicsResource* resWireframeIndices;
	GLuint numWireframeIndices;
//Normal Visualization Vertices (glDrawArrays as GL_LINES)
	GLuint vaoNormalPositions;
	GLuint vboNormalPositions;
	cudaGraphicsResource* resNormalPositions;
	GLuint numNormalPositions;
//Material Properties (pass these to Fragment Shader)
	glm::vec3 Ka, Ks, Kd;	//Lambertian Material properties: Ambient, Specular, Diffuse
	float Shininess;
public:
	std::vector<ILContainer *> textures;

public:
	Mesh(void);
	~Mesh(void);
	
	void CalculateBlockGridSize(dim3 & block, dim3 & grid, bool iterateFaces = false, bool heightDoubled = false);
	void CreatePlanarMesh(int width, int height);
	void CreateSphereMesh();
	void FixSphereEdgeCases();
	void CreateRibbon(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
	void CreateStaircase(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
	void CreateIndices();
	void CreateWireframeIndices();
	void CalculateNormals();
	void CreateNormalsVisualization();
	void CreateTextureCoords();

	void MorphMesh(glm::ivec2 point, float morphRadius, float morphHeight);

	void Initialize(int width, int height);
	void Draw(GLSLProgram & shader);
	void DrawWireframe(GLSLProgram & shader);
	void DrawNormals();
	void TakeDown();

	bool wireframeMode;
	bool drawNormals;
};

