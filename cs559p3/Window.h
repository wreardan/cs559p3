#pragma once

#include <glm/glm.hpp>

class Window
{
public:
	Window(void);
	~Window(void);

	glm::ivec2 size;	//.x = width, .y = height
	int handle;	//Window's handle
};

