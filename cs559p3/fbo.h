#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class FrameBufferObject
{
public:
	FrameBufferObject();
	void Bind(int color_buffer_index = 0);
	void Unbind();
	bool Initialize(glm::ivec2 size, int number_of_color_attachments = 1, bool allocate_depth_buffer = true);
	void TakeDown();

	glm::ivec2 size;
	int number_of_color_attachments;
	GLuint framebuffer_handle;
	GLuint depth_buffer_handle;
	GLuint * texture_handles;
};
