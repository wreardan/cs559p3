#include "PostProcessing.h"

#include <iostream>

using namespace std;
using namespace glm;

PostProcessing::PostProcessing(void)
{
	currentEffect = 0;
}


PostProcessing::~PostProcessing(void)
{
}

void PostProcessing::Initialize(glm::ivec2 & windowSize)
{
	this->windowSize = windowSize;

	//Initialization for Framebuffer stuff
	fbo.Initialize(windowSize);
	
	//Compile TextureOnly Shader
	if(!postProcessShader.compileShaderFromFile("Shaders/PostProcess.vert", GLSLShader::VERTEX))
		cerr << "Vertex Shader failed to compile: " << postProcessShader.log() << endl;
	if(!postProcessShader.compileShaderFromFile("Shaders/PostProcess.frag", GLSLShader::FRAGMENT))
		cerr << "Fragment Shader failed to compile: " << postProcessShader.log() << endl;
	if(!postProcessShader.link())
		cerr << "Shader Program failed to link: " << postProcessShader.log() << endl;
	if(!postProcessShader.validate())
		cerr << "Shader Program did not validate: " << postProcessShader.log() << endl;

	
	//Generate VAO and VBO
	glGenVertexArrays(1, &framebufferSquareVAO);
	glBindVertexArray(framebufferSquareVAO);

	float square[] = {-1.0f,  1.0f,  // 0, Top Left
          -1.0f, -1.0f,  // 1, Bottom Left
           1.0f, -1.0f,  // 2, Bottom Right
           1.0f,  1.0f,  // 3, Top Right
	};
	glGenBuffers(1, &framebufferSquareHandle);
	glBindBuffer(GL_ARRAY_BUFFER, framebufferSquareHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2(1.0f)) * 4, square, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2(1.0f)), 0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Get Subroutines
	effectSubroutines.push_back(glGetSubroutineIndex(postProcessShader.getHandle(), GL_FRAGMENT_SHADER, "drawTextureOnly"));
	effectSubroutines.push_back(glGetSubroutineIndex(postProcessShader.getHandle(), GL_FRAGMENT_SHADER, "drawPosterized"));
}

void PostProcessing::ChangeEffect()
{
	currentEffect = (currentEffect + 1) % effectSubroutines.size();
}


void PostProcessing::Bind()
{
	fbo.Bind();
}

void PostProcessing::UnBind()
{
	fbo.Unbind();
}

void PostProcessing::Draw(float time)
{

	//Clear screen
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set Viewport
	glViewport(0, 0, windowSize.x, windowSize.y);
	mat4 ViewMatrix = lookAt(vec3(0, 0, 5.5), vec3(0, 0, 0), vec3(0, 1, 0));
	
	postProcessShader.use();
	//Change Subroutine
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &effectSubroutines[currentEffect]);

	vec2 windowSizeFloat = vec2(windowSize.x, windowSize.y);
	postProcessShader.setUniform("size", windowSizeFloat);
	postProcessShader.setUniform("ColorMap", (int)0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo.texture_handles[0]);
	
	glBindVertexArray(framebufferSquareVAO);
	glDrawArrays(GL_QUADS, 0, 4);
	glBindVertexArray(0);

	postProcessShader.unuse();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}
