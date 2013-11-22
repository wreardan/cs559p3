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
	{
		block.x = 1; block.y = 1;
	}
    dim3 grid(width / block.x, height / block.y, 1);

    FillPlanarMeshKernel<<< grid, block>>>(dptr, width, height);

	cudaGraphicsUnmapResources(1, &resPosition, 0);
}

//This kernel will create a Sphere Mesh
__global__ void FillSphereMesh(float3 *pos, unsigned int width, unsigned int height, float thetaFac, float phiFac)
{

    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	float theta, phi;
	float nx, ny, nz, s, t;

	theta = y * thetaFac;
    s = y / (float)width;
	phi = x * phiFac;
    t = x / (float)height;

	nx = sinf(phi) * cosf(theta);
	ny = sinf(phi) * sinf(theta);
	nz = cosf(phi);

    pos[y*width+x] = make_float3(nx, ny, nz);

	
#if __CUDA_ARCH__ >= 200
	//printf("(%d,%d): Position: (%f,%f,%f)\n", x, y, nx, ny, nz);
#endif
}

void Mesh::CreateSphereMesh()
{
	float3* dptr;

	cudaGraphicsMapResources(1, &resPosition, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&dptr, &num_bytes, resPosition);
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	dim3 block(4, 4, 1);
	if(width % 4 || height % 4)
	{
		block.x = 1; block.y = 1;
	}
    dim3 grid(width / block.x, height / block.y, 1);
	

	GLfloat thetaFac = (2.0f * M_PI ) / (width-1);
	GLfloat phiFac = M_PI  / (height-1);
    FillSphereMesh<<< grid, block>>>(dptr, width, height, thetaFac, phiFac);

	cudaGraphicsUnmapResources(1, &resPosition, 0);
}


//This kernel computes the indices for the triangles in the mesh. 
__global__ void FillIndicesKernel(int* indices, int width, int height)
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
}

void Mesh::CreateIndices() {
	int* ptrIndices;

	cudaGraphicsMapResources(1, &resIndices, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&ptrIndices, &num_bytes, resIndices);
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	dim3 block(1, 1, 1);
    dim3 grid(width-1, height-1, 1);

	FillIndicesKernel<<< grid, block>>>(ptrIndices, width, height);

	cudaGraphicsUnmapResources(1, &resIndices, 0);
}


//This kernel computes the indices for the triangles in the mesh. 
__global__ void FillWireframeIndicesKernel(int* wireframeIndices, int width, int height)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	//Fill Wireframe indices buffer
	int wireframeOffset = (y*(width-1) + x) * 4;	//every index = 4 vertex indices (2 line segments)
	wireframeIndices[wireframeOffset++] = y * width + x;
	wireframeIndices[wireframeOffset++] = y * width + (x+1);
	wireframeIndices[wireframeOffset++] = y * width + x;
	wireframeIndices[wireframeOffset++] = (y+1) * width + x;
}
__global__ void FillWireframeIndicesEdgeCasesKernel(int* wireframeIndices, int width, int height)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	//Fill Wireframe indices buffer
	int wireframeOffset = (y*(width-1) + x) * 4;	//every index = 4 vertex indices (2 line segments)
	wireframeIndices[wireframeOffset++] = y * width + x;
	wireframeIndices[wireframeOffset++] = y * width + (x+1);
	wireframeIndices[wireframeOffset++] = y * width + x;
	wireframeIndices[wireframeOffset++] = (y+1) * width + x;
}

void Mesh::CreateWireframeIndices() {
	int* ptrWireframeIndices;

	cudaGraphicsMapResources(1, &resWireframeIndices, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&ptrWireframeIndices, &num_bytes, resWireframeIndices);
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	dim3 block(1, 1, 1);
	dim3 grid(width-1, height-1, 1);  //TODO: Fix this so that it uses (width, height) and then handles edge cases inside kernel

	FillWireframeIndicesKernel<<< grid, block>>>(ptrWireframeIndices, width, height);

	cudaGraphicsUnmapResources(1, &resWireframeIndices, 0);
}

//This kernel computes normals for the mesh
__global__ void CalculateNormalsKernel(glm::vec3* positions, glm::vec3* normals, int* indices)
{
    unsigned int index = (blockIdx.x*blockDim.x + threadIdx.x) * 3;

	

	const vec3 & p1 = positions[indices[index]];
	const vec3 & p2 = positions[indices[index+1]];
	const vec3 & p3 = positions[indices[index+2]];

	vec3 a = p2 - p1;
	vec3 b = p3 - p1;
	vec3 n = glm::cross(a,b);
	if(length(n) > 0.0f)
		n =  glm::normalize(n);
	n = -n;

#if __CUDA_ARCH__ >= 200

	atomicAdd(&normals[indices[index]].x, n.x);
	atomicAdd(&normals[indices[index]].y, n.y);
	atomicAdd(&normals[indices[index]].z, n.z);

	atomicAdd(&normals[indices[index+1]].x, n.x);
	atomicAdd(&normals[indices[index+1]].y, n.y);
	atomicAdd(&normals[indices[index+1]].z, n.z);

	atomicAdd(&normals[indices[index+2]].x, n.x);
	atomicAdd(&normals[indices[index+2]].y, n.y);
	atomicAdd(&normals[indices[index+2]].z, n.z);
//#else

	
	//normals[indices[index]] += n;  //Does this need to be atomic?  i.e. atomicAdd()? see above code for possible solution
	//normals[indices[index+1]] += n;
	//normals[indices[index+2]] += n;

	/*printf("%d: (%f,%f,%f), (%f,%f,%f), (%f,%f,%f)\n", index/3,
		normals[indices[index]].x, normals[indices[index]].y, normals[indices[index]].z,
		normals[indices[index+1]].x, normals[indices[index+1]].y, normals[indices[index+1]].z,
		normals[indices[index+2]].x, normals[indices[index+2]].y, normals[indices[index+2]].z);*/

	//printf("Block (%d): Normal: (%f,%f,%f) ", index/3, n.x, n.y, n.z);
#endif

}

__global__ void NormalizeNormals(glm::vec3* normals, unsigned int width)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	unsigned int index = y * width + x;

	if(glm::length(normals[index]) > 0.0f)
		normals[index] = glm::normalize(normals[index]);

#if __CUDA_ARCH__ >= 200
	//printf("(%d,%d): Normal: (%f,%f,%f)\n", x, y, normals[index].x, normals[index].y, normals[index].z);
#endif
}

__global__ void ZeroBufferVec3(glm::vec3* buffer, unsigned int width)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	buffer[y * width + x] = vec3(0.0f);
}


void Mesh::CalculateNormals()
{
	size_t num_bytes;
	int* indices;
	vec3* positions, *normals;

	//Map resources and get pointers
	cudaGraphicsMapResources(1, &resIndices, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&indices, &num_bytes, resIndices);
	cudaGraphicsMapResources(1, &resPosition, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&positions, &num_bytes, resPosition);
	cudaGraphicsMapResources(1, &resNormals, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&normals, &num_bytes, resNormals);

	//First, zero normals
	dim3 block(8, 8, 1);
	if(width % 8 || height % 8)
	{
		block.x = 1; block.y = 1;
	}
    dim3 grid(width / block.x, height / block.y, 1);
	ZeroBufferVec3<<< grid, block>>>(normals, width);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete
	
	//Second, generate averaged normals based on faces
	block = dim3(1,1,1);
    grid = dim3(numIndices / 3 / block.x, 1, 1);
	CalculateNormalsKernel<<< grid, block>>>(positions, normals, indices);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete

	
    //Lastly, Normalize
	block = dim3(8,8,1);
	if(width % 8 || height % 8)
	{
		block.x = 1; block.y = 1;
	}
    grid = dim3(width / block.x, height / block.y, 1);
	NormalizeNormals<<< grid, block>>>(normals, width);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete

	//Unmap Resources
	cudaGraphicsUnmapResources(1, &resNormals, 0);
	cudaGraphicsUnmapResources(1, &resPosition, 0);
	cudaGraphicsUnmapResources(1, &resIndices, 0);
}

//This kernel creates a VBO of Positions that will draw Normal geometry lines
__global__ void CreateNormalsVisualizationKernel(glm::vec3* positions, glm::vec3* normals, glm::vec3* normalPositions,
												 unsigned int * normalIndices, unsigned int width)
{
	const float normalScalar = 0.05f;

    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	unsigned int index = (y * width + x) * 2;

	glm::vec3& position = positions[y * width + x];
	glm::vec3& normal = normals[y * width + x];

	normalPositions[index] = position;
	normalPositions[index+1] = position + normal*normalScalar;

	normalIndices[index] = index;
	normalIndices[index+1] = index+1;
	
#if __CUDA_ARCH__ >= 200
	/*printf("(%d,%d): Positions: (%f,%f,%f) (%f,%f,%f)\n", x, y,
		normalPositions[index].x, normalPositions[index].y, normalPositions[index].z,
		normalPositions[index+1].x, normalPositions[index+1].y, normalPositions[index+1].z);*/
#endif
}


void Mesh::CreateNormalsVisualization()
{
	size_t num_bytes;
	vec3* positions, *normals, *normalPositions;
	unsigned int * normalIndices;

	cudaGraphicsMapResources(1, &resPosition, 0);
	cudaGraphicsMapResources(1, &resNormals, 0);
	cudaGraphicsMapResources(1, &resNormalPositions, 0);
	cudaGraphicsMapResources(1, &resNormalIndices, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&positions, &num_bytes, resPosition);
	cudaGraphicsResourceGetMappedPointer((void **)&normals, &num_bytes, resNormals);
	cudaGraphicsResourceGetMappedPointer((void **)&normalPositions, &num_bytes, resNormalPositions);
	cudaGraphicsResourceGetMappedPointer((void **)&normalIndices, &num_bytes, resNormalIndices);
	
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	dim3 block(8, 8, 1);
	if(width % 8 || height % 8)
	{
		block.x = 1; block.y = 1;
	}
    dim3 grid(width / block.x, height / block.y, 1);

	CreateNormalsVisualizationKernel<<< grid, block>>>(positions, normals, normalPositions, normalIndices, width);
	
	cudaGraphicsUnmapResources(1, &resNormalIndices, 0);
	cudaGraphicsUnmapResources(1, &resNormalPositions, 0);
	cudaGraphicsUnmapResources(1, &resNormals, 0);
	cudaGraphicsUnmapResources(1, &resPosition, 0);
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
	drawNormals = false;
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
	this->numNormalPositions = width * height * 2;
	
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
	
    cudaGraphicsGLRegisterBuffer(&resNormals, vboNormals, cudaGraphicsMapFlagsWriteDiscard);
	//Associate with Shader layout element 1, enable
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3(1.0f)), 0);
	glEnableVertexAttribArray(1);

	//Unbind array buffer and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


//Create Indices VBO
	glGenBuffers(1, &vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
	size_t bytesIndices = numIndices * sizeof(GLuint);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesIndices, 0, GL_STATIC_DRAW);
	
    cudaGraphicsGLRegisterBuffer(&resIndices, vboIndices, cudaGraphicsMapFlagsWriteDiscard);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);    //Unbind array element buffer
	

//Create Wireframe Indices VBO
	glGenBuffers(1, &vboWireframeIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboWireframeIndices);
	bytesIndices = numWireframeIndices * sizeof(GLuint);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesIndices, 0, GL_STATIC_DRAW);
	
    cudaGraphicsGLRegisterBuffer(&resWireframeIndices, vboWireframeIndices, cudaGraphicsMapFlagsWriteDiscard);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);    //Unbind array element buffer
	

//Create Normal Visualization VAO
	glGenVertexArrays(1, &vaoNormalPositions);
	glBindVertexArray(vaoNormalPositions);

//Create Normal Visualization VBO - do we need a VAO? yes
	glGenBuffers(1, &vboNormalPositions);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormalPositions);
	size_t bytesNormalPositions = sizeof(vec3(1.0f)) * width * height * 2;
	glBufferData(GL_ARRAY_BUFFER, bytesNormalPositions, 0, GL_STATIC_DRAW);
	
    cudaGraphicsGLRegisterBuffer(&resNormalPositions, vboNormalPositions, cudaGraphicsMapFlagsWriteDiscard);
//Associate with Shader layout element 0, enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3(1.0f)), 0);
	glEnableVertexAttribArray(0);

//Unbind array buffer and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

//Create Normal Indices VBO
	glGenBuffers(1, &vboNormalIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboNormalIndices);
	size_t bytesNormalIndices = numNormalPositions * sizeof(GLuint);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesNormalIndices, 0, GL_STATIC_DRAW);
	
    cudaGraphicsGLRegisterBuffer(&resNormalIndices, vboNormalIndices, cudaGraphicsMapFlagsWriteDiscard);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);    //Unbind array element buffer

	
	//Create basic planar mesh and indices
	//CreatePlanarMesh(width, height);
	CreateSphereMesh();

	CreateIndices();
	CreateWireframeIndices();

	//some test fucntions:
	CalculateNormals();
	cudaDeviceSynchronize();	//http://stackoverflow.com/questions/15669841/cuda-hello-world-printf-not-working-even-with-arch-sm-20
	CreateNormalsVisualization();
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

void Mesh::DrawNormals()
{
	glBindVertexArray(vaoNormalPositions);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboNormalIndices);
	glDrawElements(GL_LINES, numNormalPositions, GL_UNSIGNED_INT, (GLvoid*)0);
	glBindVertexArray(0);
}



void Mesh::TakeDown()
{
	//TODO: De-Allocate GL objects here
}
