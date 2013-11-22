#include "Mesh.h"


#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309616

using namespace glm;

//This kernel fills the Planar Mesh's Vertex Positions
__global__ void FillPlanarMeshKernel(float3 *pos, unsigned int width, unsigned int height)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	float x_coord = x/((float)(width-1)/2) - 1;
	float y_coord = y/((float)(height-1)/2) - 1;
    pos[y*width+x] = make_float3(x_coord, y_coord, 0.0f);
}


void Mesh::CreatePlanarMesh(int width, int height)
{
	float3* dptr;

	cudaGraphicsMapResources(1, &resPosition, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&dptr, &num_bytes, resPosition);
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	dim3 block(8, 8, 1);
	if(width % 8 || height % 8)
		block.x = 1; block.y = 1;
    dim3 grid(width / block.x, height / block.y, 1);

    FillPlanarMeshKernel<<< grid, block>>>(dptr, width, height);

	cudaGraphicsUnmapResources(1, &resPosition, 0);
}

//This kernel will create a Sphere Mesh
__global__ void FillSphereMesh(float3 *pos, unsigned int width, unsigned int height, float thetaFac, float phiFac)
{

    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	GLfloat theta, phi;
	GLfloat nx, ny, nz, s, t;

	theta = y * thetaFac;
    s = (GLfloat)y / width;
	phi = x * phiFac;
    t = (GLfloat)x / height;

	nx = sinf(phi) * cosf(theta);
	ny = sinf(phi) * sinf(theta);
	nz = cosf(phi);

    pos[y*width+x] = make_float3(nx, ny, nz);
}

void Mesh::CreateSphereMesh(int width, int height)
{
	float3* dptr;

	cudaGraphicsMapResources(1, &resPosition, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&dptr, &num_bytes, resPosition);
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	dim3 block(8, 8, 1);
	if(width % 8 || height % 8)
		block.x = 1; block.y = 1;
    dim3 grid(width / block.x, height / block.y, 1);
	

	GLfloat thetaFac = (2.0f * M_PI ) / (width-1);
	GLfloat phiFac = M_PI  / (height-1);
    FillSphereMesh<<< grid, block>>>(dptr, width, height, thetaFac, phiFac);

	cudaGraphicsUnmapResources(1, &resPosition, 0);
}


//This kernel computes the indices for the triangles in the mesh. 
__global__ void FillIndicesKernel(int* indices, int* wireframeIndices, int width, int height)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	//Fill Indices buffer
	int indexOffset = (y*(width-1) + x) * 6;	//every index = 6 vertex indices (2 triangles)
	
	indices[indexOffset++] = (y) * width + (x+1);
	indices[indexOffset++] = (y) * width + x;
	indices[indexOffset++] = (y+1) * width + x;

	indices[indexOffset++] = (y+1) * width + x;
	indices[indexOffset++] = (y+1) * width + (x+1);
	indices[indexOffset++] = y * width + (x+1);

	//Fill Wireframe indices buffer
	int wireframeOffset = (y*(width-1) + x) * 4;	//every index = 4 vertex indices (2 line segments)
	wireframeIndices[wireframeOffset++] = y * width + x;
	wireframeIndices[wireframeOffset++] = y * width + (x+1);
	wireframeIndices[wireframeOffset++] = y * width + x;
	wireframeIndices[wireframeOffset++] = (y+1) * width + x;
}

void Mesh::CreateIndices(int width, int height) {
	int* ptrIndices, *ptrWireframeIndices;

	cudaGraphicsMapResources(1, &resIndices, 0);
	cudaGraphicsMapResources(1, &resWireframeIndices, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&ptrIndices, &num_bytes, resIndices);
	cudaGraphicsResourceGetMappedPointer((void **)&ptrWireframeIndices, &num_bytes, resWireframeIndices);
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	dim3 block(1, 1, 1);
    dim3 grid(width-1, height-1, 1);

	FillIndicesKernel<<< grid, block>>>(ptrIndices, ptrWireframeIndices, width, height);

	cudaGraphicsUnmapResources(1, &resIndices, 0);
	cudaGraphicsUnmapResources(1, &resWireframeIndices, 0);
}

//This kernel computes normals for the mesh
__global__ void CalculateNormalsKernel(glm::vec3* positions, glm::vec3* normals, int* indices, int width, int height)
{
	
}

void Mesh::CalculateNormals()
{
	size_t num_bytes;
	int* indices;
	vec3* positions, *normals;

	cudaGraphicsMapResources(1, &resIndices, 0);
	cudaGraphicsMapResources(1, &resPosition, 0);
	//cudaGraphicsMapResources(1, &resNormals, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&indices, &num_bytes, resIndices);
	
	dim3 block(8, 8, 1);
	if(width % 8 || height % 8)
		block.x = 1; block.y = 1;
    dim3 grid(width / block.x, height / block.y, 1);

	CalculateNormalsKernel<<< grid, block>>>(positions, normals, indices, width, height);

	cudaGraphicsUnmapResources(1, &resIndices, 0);
}


using namespace std;
using namespace glm;

//Initialize default values
Mesh::Mesh(void)
{
	width = height = 0;

	vao = GL_BAD_VALUE;

	vboPosition = GL_BAD_VALUE;
	resPosition = NULL;
	vboNormals = GL_BAD_VALUE;
	resNormals = NULL;

	vboIndices = GL_BAD_VALUE;
	numIndices = 0;
	resIndices = NULL;

	vboWireframeIndices = GL_BAD_VALUE;
	numWireframeIndices = 0;
	resWireframeIndices = NULL;

	vboNormalPositions = GL_BAD_VALUE;
	numNormalPositions = 0;
	resNormalPositions = NULL;

	Ka = vec3(1.0f);
	Ks = vec3(1.0f);
	Kd = vec3(1.0f);
	Shininess = 100.0f;

	wireframeMode = false;
}


Mesh::~Mesh(void)
{
	//TODO: Add handle asserts here to make sure objects are de-allocated.  example:
	//assert(vao == GL_BAD_VALUE);
}



void Mesh::Initialize(int width, int height)
{
	//store the Mesh width and height
	this->width = width;
	this->height = height;

	//calculate the number of indices the Mesh will have
	this->numIndices = (width-1) * (height-1) * 6;
	this->numWireframeIndices = (width-1) * (height-1) * 4;
	
	//Create VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

//Create Position VBO
	glGenBuffers(1, &vboPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	size_t bytesPosition = sizeof(vec3) * width * height;
	glBufferData(GL_ARRAY_BUFFER, bytesPosition, 0, GL_STATIC_DRAW);
	
    cudaGraphicsGLRegisterBuffer(&resPosition, vboPosition, cudaGraphicsMapFlagsWriteDiscard);
//Associate with Shader layout element 0, enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3(1.0f)), 0);
	glEnableVertexAttribArray(0);

//Create Normal VBO
	glGenBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, bytesPosition, 0, GL_STATIC_DRAW);//same size as Position VBO, both are vec3
	
    cudaGraphicsGLRegisterBuffer(&resPosition, vboPosition, cudaGraphicsMapFlagsWriteDiscard);
//Associate with Shader layout element 1, enable
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3(1.0f)), 0);
	glEnableVertexAttribArray(1);

//Create Indices VBO
	glGenBuffers(1, &vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
	size_t bytesIndices = numIndices * sizeof(GLuint);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesIndices, 0, GL_STATIC_DRAW);
	
    cudaGraphicsGLRegisterBuffer(&resIndices, vboIndices, cudaGraphicsMapFlagsWriteDiscard);

//Unbind array buffer and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
//Create Wireframe VBO
	glGenBuffers(1, &vboWireframeIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboWireframeIndices);
	bytesIndices = numWireframeIndices * sizeof(GLuint);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesIndices, 0, GL_STATIC_DRAW);
	
    cudaGraphicsGLRegisterBuffer(&resWireframeIndices, vboWireframeIndices, cudaGraphicsMapFlagsWriteDiscard);

//Create Normal Visualization VBO
	glGenBuffers(1, &vboNormalPositions);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormalPositions);
	size_t bytesNormalPositions = sizeof(vec3(1.0f)) * width * height * 2;
	glBufferData(GL_ARRAY_BUFFER, bytesNormalPositions, 0, GL_STATIC_DRAW);
	
    cudaGraphicsGLRegisterBuffer(&resPosition, vboPosition, cudaGraphicsMapFlagsWriteDiscard);

	
	//Create basic planar mesh and indices
	//CreatePlanarMesh(width, height);
	CreateSphereMesh(width, height);
	CreateIndices(width, height);
}

void Mesh::Draw()
{
	glBindVertexArray(vao);
	if(wireframeMode) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboWireframeIndices);
		glDrawElements(GL_LINES, numWireframeIndices, GL_UNSIGNED_INT, (GLvoid*)0);
	} else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
		glDrawElements(GL_TRIANGLES, numIndices*8/8, GL_UNSIGNED_INT, (GLvoid*)0);
	}
	glBindVertexArray(0);
}

void Mesh::TakeDown()
{
	//TODO: De-Allocate GL objects here
}
