#include "Mesh.h"


#define M_PI 3.14159265358979323846f
#define M_PI_2 1.57079632679489661923f
#define M_PI_4 0.785398163397448309616f

using namespace std;
using namespace glm;

//http://rosettacode.org/wiki/Factors_of_an_integer#C.2B.2B
//we use factors to break problem into sub-problems
int maxFactor(int n, int maxValue)
{
	for(int i = 2; i * i <= n; i++)
		if(n % i == 0)
			if(n / i < maxValue)
				return n / i;
	if(n > maxValue)
		return 1;
	return n;
}
void Mesh::CalculateBlockGridSize(dim3 & block, dim3 & grid, bool iterateFaces, bool heightDoubled)
{
	block = dim3(1, 1, 1);
	if(iterateFaces)
	{
		block.x = maxFactor(width-1, 512);
		block.y = maxFactor(height-1, 512/block.x);
		grid = dim3((width-1)/block.x, (height-1)/block.y, 1);
	}
	else
	{
		block.x = maxFactor(width, 512);
		block.y = maxFactor(height, 512/block.x);
		grid = dim3(width/block.x, height/block.y, 1);
	}
	if(heightDoubled)
		grid.y *= 2;
}


//http://stackoverflow.com/questions/14038589/what-is-the-canonical-way-to-check-for-errors-using-the-cuda-runtime-api
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

//http://stackoverflow.com/questions/4997013/cuda-kernels-consistently-returning-bad-results
void CheckErrorCUDA()
{
	cudaError_t cudaResult;
	cudaResult = cudaGetLastError();
	if (cudaResult != cudaSuccess)
		printf(cudaGetErrorString(cudaResult));
}

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
	dim3 block, grid;

	cudaGraphicsMapResources(1, &resPosition, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&dptr, &num_bytes, resPosition);
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	CalculateBlockGridSize(block, grid);

    FillPlanarMeshKernel<<< grid, block>>>(dptr, width, height);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete

	cudaGraphicsUnmapResources(1, &resPosition, 0);
}

//This kernel will create a Sphere Mesh
__global__ void FillSphereMesh(float3 *pos, unsigned int width, unsigned int height, float thetaFac, float phiFac)
{

    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	float theta, phi;
	float nx, ny, nz;

	theta = y * thetaFac;
	phi = x * phiFac;

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
	dim3 block, grid;

	cudaGraphicsMapResources(1, &resPosition, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&dptr, &num_bytes, resPosition);
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	CalculateBlockGridSize(block, grid);

	GLfloat thetaFac = (2.0f * M_PI ) / (width-1);
	GLfloat phiFac = M_PI  / (height-1);
    FillSphereMesh<<< grid, block>>>(dptr, width, height, thetaFac, phiFac);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete

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
	dim3 block, grid;

	cudaGraphicsMapResources(1, &resIndices, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&ptrIndices, &num_bytes, resIndices);
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	CalculateBlockGridSize(block, grid, true);

	FillIndicesKernel<<< grid, block>>>(ptrIndices, width, height);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete

	cudaGraphicsUnmapResources(1, &resIndices, 0);
}


//This kernel computes the indices for the triangles in the mesh. 
__global__ void FillWireframeIndicesKernel(int* wireframeIndices, int width, int height)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	//Fill Wireframe indices buffer
	int wireframeOffset = (y*(width) + x) * 4;	//every index = 4 vertex indices (2 line segments)
	wireframeIndices[wireframeOffset++] = y * width + x;
	wireframeIndices[wireframeOffset++] = y * width + (x+1);
	wireframeIndices[wireframeOffset++] = y * width + x;
	wireframeIndices[wireframeOffset++] = (y+1) * width + x;
}
__global__ void FillWireframeIndicesEdgeCase1Kernel(int* wireframeIndices, int width, int height)
{   //Handle the 'top' edge case
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = height-1;

	//Fill Wireframe indices buffer
	int wireframeOffset = (y*(width) + x) * 4;	//every index = 4 vertex indices (2 line segments)

	wireframeIndices[wireframeOffset++] = y * width + x;
	wireframeIndices[wireframeOffset++] = y * width + (x+1);
}
__global__ void FillWireframeIndicesEdgeCase2Kernel(int* wireframeIndices, int width, int height)
{   //Handle the 'right' edge case
    unsigned int x = width-1;
    unsigned int y = blockIdx.x*blockDim.x + threadIdx.x;	//use x instead of y for the index

	//Fill Wireframe indices buffer
	int wireframeOffset = (y*(width) + x) * 4;	//every index = 4 vertex indices (2 line segments)

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

	//Handle edge cases
	grid.y = 1;
	FillWireframeIndicesEdgeCase1Kernel<<< grid, block>>>(ptrWireframeIndices, width, height);

	grid.x = height-1;
	FillWireframeIndicesEdgeCase2Kernel<<< grid, block>>>(ptrWireframeIndices, width, height);

	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete, only need once since all kernels can be run in parallel

	cudaGraphicsUnmapResources(1, &resWireframeIndices, 0);
}

//Compute Level 1.1 atomicAdd workaround (slow)
//https://www.sharcnet.ca/help/index.php/CUDA_tips_and_tricks
__device__ inline void MyAtomicAdd (float *address, float value)
 {
   int oldval, newval, readback;
 
   oldval = __float_as_int(*address);
   newval = __float_as_int(__int_as_float(oldval) + value);
   while ((readback=atomicCAS((int *)address, oldval, newval)) != oldval) 
     {
      oldval = readback;
      newval = __float_as_int(__int_as_float(oldval) + value);
     }
 }

//add b to a atomically
__device__ inline void AtomicAddvec3(glm::vec3* a, glm::vec3* b)
{
#if __CUDA_ARCH__ >= 200
	atomicAdd(&a->x, b->x);
	atomicAdd(&a->y, b->y);
	atomicAdd(&a->z, b->z);
#else
	MyAtomicAdd(&a->x, b->x);
	MyAtomicAdd(&a->y, b->y);
	MyAtomicAdd(&a->z, b->z);
#endif
}


//This kernel computes normals for the mesh by computing the cross product for each triangle, then adding that value to each vertex
__global__ void CalculateNormalsKernel(glm::vec3* positions, glm::vec3* normals, int* indices, unsigned int width)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	unsigned int index = (y * width + x) * 3;

	const vec3 & p1 = positions[indices[index]];
	const vec3 & p2 = positions[indices[index+1]];
	const vec3 & p3 = positions[indices[index+2]];

	vec3 a = p2 - p1;
	vec3 b = p3 - p1;
	vec3 n = glm::cross(a,b);
	if(length(n) > 0.0f)
		n =  glm::normalize(n);
	n = -n;		//why do we need this??

	AtomicAddvec3(&normals[indices[index]], &n);
	AtomicAddvec3(&normals[indices[index+1]], &n);
	AtomicAddvec3(&normals[indices[index+2]], &n);

}

__global__ void NormalizeNormals(glm::vec3* normals, unsigned int width)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	unsigned int index = y * width + x;

	if(glm::length(normals[index]) > 0.0f)
		normals[index] = glm::normalize(normals[index]);
	else
		normals[index] = vec3(0,0,-10);

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
	dim3 block, grid;

	//Map resources and get pointers
	cudaGraphicsMapResources(1, &resIndices, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&indices, &num_bytes, resIndices);
	cudaGraphicsMapResources(1, &resPosition, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&positions, &num_bytes, resPosition);
	cudaGraphicsMapResources(1, &resNormals, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&normals, &num_bytes, resNormals);

	//First, zero normals
	CalculateBlockGridSize(block, grid);
	ZeroBufferVec3<<< grid, block>>>(normals, width);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete
	
	//Second, generate averaged normals based on faces
	CalculateBlockGridSize(block, grid, true, true);
	CalculateNormalsKernel<<< grid, block>>>(positions, normals, indices, width-1);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete
	CheckErrorCUDA();
	
    //Lastly, Normalize
	CalculateBlockGridSize(block, grid);
	NormalizeNormals<<< grid, block>>>(normals, width);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete

	//Unmap Resources
	cudaGraphicsUnmapResources(1, &resNormals, 0);
	cudaGraphicsUnmapResources(1, &resPosition, 0);
	cudaGraphicsUnmapResources(1, &resIndices, 0);
}

//This kernel creates a VBO of Positions that will draw Normal geometry lines
__global__ void CreateNormalsVisualizationKernel(glm::vec3* positions, glm::vec3* normals, glm::vec3* normalPositions, unsigned int width)
{
	const float normalScalar = 0.05f;

    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

	unsigned int index = (y * width + x) * 2;

	glm::vec3& position = positions[y * width + x];
	glm::vec3& normal = normals[y * width + x];

	normalPositions[index] = position;
	normalPositions[index+1] = position + normal*normalScalar;
	
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
	dim3 block, grid;

	cudaGraphicsMapResources(1, &resPosition, 0);
	cudaGraphicsMapResources(1, &resNormals, 0);
	cudaGraphicsMapResources(1, &resNormalPositions, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&positions, &num_bytes, resPosition);
	cudaGraphicsResourceGetMappedPointer((void **)&normals, &num_bytes, resNormals);
	cudaGraphicsResourceGetMappedPointer((void **)&normalPositions, &num_bytes, resNormalPositions);
	
	
    // execute the kernel - performance improvement when width and height are divisible by 8
	CalculateBlockGridSize(block, grid);
	CreateNormalsVisualizationKernel<<< grid, block>>>(positions, normals, normalPositions, width);
	cudaDeviceSynchronize();  //Wait for CUDA kernel to Complete
	
	cudaGraphicsUnmapResources(1, &resNormalPositions, 0);
	cudaGraphicsUnmapResources(1, &resNormals, 0);
	cudaGraphicsUnmapResources(1, &resPosition, 0);
}

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
	this->numWireframeIndices = (width) * (height) * 4;
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
	size_t bytesNormalPositions = numNormalPositions * sizeof(vec3(1.0f));
	glBufferData(GL_ARRAY_BUFFER, bytesNormalPositions, 0, GL_STATIC_DRAW);
	
    cudaGraphicsGLRegisterBuffer(&resNormalPositions, vboNormalPositions, cudaGraphicsMapFlagsWriteDiscard);
//Associate with Shader layout element 0, enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3(1.0f)), 0);
	glEnableVertexAttribArray(0);

//Unbind array buffer and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
	//Create basic planar mesh and indices
	CreatePlanarMesh(width, height);
	//CreateSphereMesh();

	CreateIndices();
	CreateWireframeIndices();

	//some test fucntions:
	CalculateNormals();
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
	glDrawArrays(GL_LINES, 0, numNormalPositions);
	glBindVertexArray(0);
}



void Mesh::TakeDown()
{
	//TODO: De-Allocate GL objects here
}
