#pragma once
#include "fbo.h"
#include "glslprogram.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
class PostProcessing
{
protected:
	FrameBufferObject fbo;
	GLuint framebufferSquareHandle, framebufferSquareVAO;
	std::vector<GLuint> postProcessingSubroutines;
	GLSLProgram postProcessShader;
	glm::ivec2 windowSize;
	std::vector<GLuint> effectSubroutines;
	int currentEffect;

public:
	PostProcessing(void);
	~PostProcessing(void);

	void Initialize(glm::ivec2 & windowSize);
	void Bind();
	void UnBind();
	void Draw(float time);
	void ChangeEffect();
};

