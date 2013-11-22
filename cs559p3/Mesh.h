#pragma once

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <GL/glew.h>
#include <cuda.h>
#include <cuda_gl_interop.h>
#include <glm/glm.hpp>

#ifndef GL_BAD_VALUE
#define GL_BAD_VALUE (-1)
#endif

class Mesh
{
protected:
//Mesh Properties
	int width, height;
//Vertex Attributes
	GLuint vao;

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
	GLuint vboNormalPositions;
	cudaGraphicsResource* resNormalPositions;
	GLuint numNormalPositions;
//Material Properties (pass these to Fragment Shader)
	glm::vec3 Ka, Ks, Kd;	//Lambertian Material properties: Ambient, Specular, Diffuse
	float Shininess;

public:
	Mesh(void);
	~Mesh(void);

	void CreatePlanarMesh(int width, int height);
	void CreateSphereMesh(int width, int height);
	void CreateIndices(int width, int height);
	void CalculateNormals();

	void Initialize(int width, int height);
	void Draw();
	void TakeDown();

	bool wireframeMode;
};

