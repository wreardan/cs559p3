/* Perry Kivolowitz */

#pragma once
#undef	_UNICODE
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#define	BAD_IL_VALUE	IL_INVALID_VALUE
#ifndef BAD_GL_VALUE
#define	BAD_GL_VALUE	GLuint(-1)
#endif // !BAD_GL_VALUE

/*	This class assumes that the object will be dynamically created.
	This  assumption is  demonstrated in the destructor which calls
	OpenGL. If an object  is  declared  in  the  global  score, the 
	destructor will run  after  the  OpenGL  context is  destroyed, 
	which would be an error.
*/

class ILContainer
{
public:
	ILContainer()
	{
		this->il_handle = BAD_IL_VALUE;
		this->il_texture_handle = BAD_GL_VALUE;
	}

	~ILContainer();
	
	bool Initialize(const char * file_name);
	bool Initialize();
	void Bind(GLuint texture_unit = 0);

	ILint width;
	ILint height;
	ILint format;
	ILuint il_handle;
	GLuint il_texture_handle;
	GLvoid * data;
};

/* Do not use

class ILCubemapContainer
{
public:
	ILCubemapContainer()
	{
		this->il_handle = BAD_IL_VALUE;
		this->il_texture_handle[0] = BAD_GL_VALUE;
	}

	~ILCubemapContainer();
	
	bool Initialize(const char * file_name, int width, int height, int offsetx, int offsety);
	void Bind(GLuint texture_unit = 0);

	ILint width;
	ILint height;
	ILuint il_handle;
	GLuint il_texture_handle[7];
};

*/