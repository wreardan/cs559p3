#pragma once

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <GL/glew.h>
#include <cuda.h>
#include <cuda_gl_interop.h>
#include <glm/glm.hpp>

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
//Vertex Positions
	GLuint vboPosition;
	cudaGraphicsResource* resPosition;
	GLuint vboNormals;
	cudaGraphicsResource* resNormals;
//Vertex Indices
	GLuint vboIndices;
	cudaGraphicsResource* resIndices;
	GLuint numIndices;
//Wireframe Indices (uses same Vertices)
	GLuint vboWireframeIndices;
	cudaGraphicsResource* resWireframeIndices;
	GLuint numWireframeIndices;
	//Normal Visualization Vertices (glDrawArrays as GL_LINES) -> NOT YET IMPLEMENTED
	GLuint vaoNormalPositions;
	GLuint vboNormalPositions;
	cudaGraphicsResource* resNormalPositions;
	GLuint numNormalPositions;
//Material Properties (pass these to Fragment Shader)
	glm::vec3 Ka, Ks, Kd;	//Lambertian Material properties: Ambient, Specular, Diffuse
	float Shininess;

public:
	Mesh(void);
	~Mesh(void);
	
	void CalculateBlockGridSize(dim3 & block, dim3 & grid, bool iterateFaces = false, bool heightDoubled = false);
	void CreatePlanarMesh(int width, int height);
	void CreateSphereMesh();
	void CreateIndices();
	void CreateWireframeIndices();
	void CalculateNormals();
	void CreateNormalsVisualization();

	void Initialize(int width, int height);
	void Draw();
	void DrawNormals();
	void TakeDown();

	bool wireframeMode;
	bool drawNormals;
};

