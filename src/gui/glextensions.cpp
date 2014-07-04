/*
	*** GL Extensions
	*** src/gui/glextensions.cpp
	Copyright T. Youngs 2013-2014

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/glextensions.h"
#include <string.h>

// Constructor
GLExtensions::GLExtensions() : ListItem<GLExtensions>()
{
	// Queries
	glBeginQuery = NULL;
	glEndQuery = NULL;
	glGenQueries = NULL;
	glGetQueryObjectiv = NULL;
	glGetQueryObjectui64v = NULL;

	// VBOs
	glGenBuffers = NULL;
	glBindBuffer = NULL;
	glBufferData = NULL;
	glBufferSubData = NULL;
	glDeleteBuffers = NULL;

	// Query OpenGL Extensions and set features
	const GLubyte* glexts = NULL;
	glexts = glGetString(GL_EXTENSIONS);
	hasVBO_ = (strstr((const char*)glexts, "GL_ARB_vertex_buffer_object") != NULL);
	hasQueries_ = (strstr((const char*)glexts, "GL_ARB_timer_query") != NULL);

	// Grab GL function pointers
#ifdef _WIN32
	// Queries
	glBeginQuery = (PFNGLBEGINQUERYPROC) wglGetProcAddress("glBeginQuery");
	glEndQuery = (PFNGLENDQUERYPROC) wglGetProcAddress("glEndQuery");
	glGenQueries = (PFNGLGENQUERIESPROC) wglGetProcAddress("glGenQueries");
	glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC) wglGetProcAddress("glGetQueryObjectiv");
	glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC) wglGetProcAddress("glGetQueryObjectui64v");
	// VBOs
	glGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC) wglGetProcAddress("glBufferSubData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglGetProcAddress("glDeleteBuffers");
#else
	// Queries
	glBeginQuery = (PFNGLBEGINQUERYPROC) glXGetProcAddress((const GLubyte*) "glBeginQuery");
	glEndQuery = (PFNGLENDQUERYPROC) glXGetProcAddress((const GLubyte*) "glEndQuery");
	glGenQueries = (PFNGLGENQUERIESPROC) glXGetProcAddress((const GLubyte*) "glGenQueries");
	glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC) glXGetProcAddress((const GLubyte*) "glGetQueryObjectiv");
	glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC) glXGetProcAddress((const GLubyte*) "glGetQueryObjectui64v");
	// VBOs
	glGenBuffers = (PFNGLGENBUFFERSPROC) glXGetProcAddress((const GLubyte*) "glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC) glXGetProcAddress((const GLubyte*) "glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC) glXGetProcAddress((const GLubyte*) "glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC) glXGetProcAddress((const GLubyte*) "glBufferSubData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) glXGetProcAddress((const GLubyte*) "glDeleteBuffers");
#endif
}

// Return whether we have the query extension
bool GLExtensions::hasQueries()
{
	return hasQueries_;
}

// Return whether we have the VBO extension
bool GLExtensions::hasVBO()
{
	return hasVBO_;
}
