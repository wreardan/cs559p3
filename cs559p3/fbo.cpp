#include <iostream>
#include <sstream>
#include "fbo.h"

using namespace std;


bool CheckGLErrors(const char * caller)
{

	GLenum e;
	bool r = true;
	
	while ((e = glGetError()) != GL_NO_ERROR)
	{
		r = false;
		cout << caller << " " << gluErrorString(e) << endl;
	}
	return r;
}
const char * FramebufferCompletenessError(GLint error_code)
{
	switch (error_code)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		return "GL_FRAMEBUFFER_UNDEFINED";

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";

	case GL_FRAMEBUFFER_COMPLETE:
		return "GL_FRAMEBUFFER_COMPLETE";

	case GL_FRAMEBUFFER_UNSUPPORTED:
		return "GL_FRAMEBUFFER_UNSUPPORTED";

	default:
		return "Unknown frame buffer error";
	}
}

/*	Frame buffer object demonstration class by Perry Kivolowitz, University of Wisconsin Madison.

	Note this class provides the option of allocating multiple color attachments per frame buffer
	object as well as the option to attach a depth buffer to the frame buffer object. 
*/

FrameBufferObject::FrameBufferObject()
{
	this->framebuffer_handle = GLuint(-1);
	this->depth_buffer_handle = GLuint(-1);
	this->texture_handles = NULL;
}

void FrameBufferObject::Bind(int color_attachment_index)
{
	assert(color_attachment_index <= this->number_of_color_attachments);
	assert(this->framebuffer_handle != GLuint(-1));
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer_handle);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + color_attachment_index);
}

void FrameBufferObject::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::TakeDown()
{
	if (this->framebuffer_handle != GLuint(-1))
		glDeleteFramebuffers(1, &this->framebuffer_handle);

	if (this->depth_buffer_handle != GLuint(-1))
		glDeleteFramebuffers(1, &this->depth_buffer_handle);

	if (this->texture_handles != NULL)
	{
		glDeleteTextures(this->number_of_color_attachments, this->texture_handles);
		delete [] this->texture_handles;
	}
}

bool FrameBufferObject::Initialize(glm::ivec2 size, int number_of_color_attachments, bool allocate_depth_buffer)
{
	this->size = size;
	this->number_of_color_attachments = number_of_color_attachments;
	this->texture_handles = new GLuint[number_of_color_attachments];

	for (int i = 0; i < number_of_color_attachments; i++)
		this->texture_handles[i] = GLuint(-1);

	if (!CheckGLErrors("FrameBufferObject::Initialize - purging errors prior to initializing frame buffer object."))
		return false;

	glGenFramebuffers(1, &this->framebuffer_handle);
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer_handle);

	if (!CheckGLErrors("FrameBufferObject::Initialize - purging errors prior to initializing textures."))
		return false;

	glGenTextures(number_of_color_attachments, this->texture_handles);
	for (int i = 0; i < number_of_color_attachments; i++)
	{
		glBindTexture(GL_TEXTURE_2D, this->texture_handles[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->size.x, this->size.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->texture_handles[i], 0);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "FrameBufferObject::Initialize failed to complete texture buffer " << FramebufferCompletenessError(status) << endl;
		return false;
	}

	if (allocate_depth_buffer)
	{
		glGenRenderbuffers(1, &this->depth_buffer_handle);
		glBindRenderbuffer(GL_RENDERBUFFER, this->depth_buffer_handle);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->size.x, this->size.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depth_buffer_handle);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "FrameBufferObject::Initialize failed to complete depth buffer " << FramebufferCompletenessError(status) << endl;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}
