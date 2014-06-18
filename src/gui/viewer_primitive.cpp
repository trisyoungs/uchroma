/*
	*** Rendering Primitive
	*** src/gui/viewer_primitive.cpp
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

#define GL_GLEXT_PROTOTYPES
#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include "glext.h"
#endif
#include "gui/viewer_primitive.h"
#include <string.h>

// Declare static VBO functions (Windows only)
#ifdef _WIN32
PFNGLGENBUFFERSPROC Primitive::glGenBuffers = NULL;
PFNGLBINDBUFFERPROC Primitive::glBindBuffer = NULL;
PFNGLBUFFERDATAPROC Primitive::glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC Primitive::glBufferSubData = NULL;
PFNGLDELETEBUFFERSPROC Primitive::glDeleteBuffers = NULL;
#endif

/*
// Primitive Instance
*/

// Constructor
PrimitiveInstance::PrimitiveInstance() : ListItem<PrimitiveInstance>()
{
	// Private variables
	context_ = NULL;
	type_ = PrimitiveInstance::ListInstance;
	listObject_ = 0;
	vboVertexObject_ = 0;
	vboIndexObject_ = 0;
}

// Set display list data
void PrimitiveInstance::setDisplayList(const QGLContext *context, GLuint listObject)
{
	context_ = context;
	type_ = PrimitiveInstance::ListInstance;
	listObject_ = listObject;
}

// Set vbo object data
void PrimitiveInstance::setVBO(const QGLContext *context, GLuint vertexObject, GLuint indexObject)
{
	context_ = context;
	type_ = PrimitiveInstance::VBOInstance;
	vboVertexObject_ = vertexObject;
	vboIndexObject_ = indexObject;
}

// Return context to which primitive instance is associated
const QGLContext *PrimitiveInstance::context()
{
	return context_;
}

// Return type of instance
PrimitiveInstance::InstanceType PrimitiveInstance::type()
{
	return type_;
}

// Return display list object for instance
GLuint PrimitiveInstance::listObject()
{
	return listObject_;
}

// Return VBO ID of vertex array for instance
GLuint PrimitiveInstance::vboVertexObject()
{
	return vboVertexObject_;
}

// Return VBO ID of index array for instance
GLuint PrimitiveInstance::vboIndexObject()
{
	return vboIndexObject_;
}

/*
// Primitive
*/

// Constructor
Primitive::Primitive() : ListItem<Primitive>()
{
	colouredVertexData_ = false;
	type_ = GL_TRIANGLES;
	useInstances_ = true;
	useIndices_ = false;
	name_ = "<UnnamedPrimitive>";
}

// Destructor
Primitive::~Primitive()
{
}

// Initialise primitive
void Primitive::initialise(int maxVertices, int maxIndices, GLenum type, bool colourData)
{
	type_ = type;
	colouredVertexData_ = colourData;
	vertexChunk_.initialise(maxVertices, maxIndices, type, colouredVertexData_);
}

// Forget all data, leaving arrays intact
void Primitive::forgetAll()
{
	vertexChunk_.forgetAll();
}

// Return number of vertices currently defined in primitive
int Primitive::nDefinedVertices()
{
	return vertexChunk_.nDefinedVertices();
}

// Return number of indices currently defined in primitive
int Primitive::nDefinedIndices()
{
	return vertexChunk_.nDefinedIndices();
}

// Set name of primitive
void Primitive::setName(const char *s)
{
	name_ = s;
}

// Return name of primitive
const char *Primitive::name()
{
	return name_.get();
}

// Return first chunk vertex array
const VertexChunk& Primitive::vertexChunk()
{
	return vertexChunk_;
}

// Return whether vertex data contains colour information
bool Primitive::colouredVertexData()
{
	return colouredVertexData_;
}

// Flag that this primitive should not use instances (rendering will use vertex arrays)
void Primitive::setNoInstances()
{
	useInstances_ = false;
}

// Set whether indexed vertex drawing is to be used
void Primitive::setUseIndices(bool b)
{
	useIndices_ = b;
}

// Push instance of primitive
void Primitive::pushInstance(const QGLContext *context)
{
	// Does this primitive use instances?
	if (!useInstances_) return;

	// Clear the error flag
	glGetError();

	// Vertex buffer object or plain old display list?
	if (Primitive::defaultInstanceType_ == PrimitiveInstance::VBOInstance)
	{
		// Prepare local array of data to pass to VBO
		int offset;
		GLuint vertexVBO, indexVBO;
		if (vertexChunk_.nDefinedVertices() <= 0)
		{
			printf("Error: No data in Primitive with which to create VBO.\n");
			msg.exit("Primitive::pushInstance");
			return;
		}
		
		// Determine total size of array (in bytes) for VBO
		int vboSize = vertexChunk_.nDefinedVertices() * (colouredVertexData_ ? 10 : 6) * sizeof(GLfloat);
		
		// Generate vertex array object
		glGenBuffers(1, &vertexVBO);

		// Bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		
		// Initialise vertex array data
		glBufferData(GL_ARRAY_BUFFER, vboSize, vertexChunk_.vertexData(), GL_STATIC_DRAW);
		if (glGetError() != GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			printf("Error occurred while generating vertex buffer object for Primitive.\n");
			glDeleteBuffers(1, &vertexVBO);
			vertexVBO = 0;
			return;
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Generate index array ovject (if using indices)
		if (useIndices_)
		{
			// Generate index array object
			glGenBuffers(1, &indexVBO);

			// Bind VBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
			
			// Initialise index array data
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexChunk_.nDefinedIndices()*sizeof(GLuint), vertexChunk_.indexData(), GL_STATIC_DRAW);
			if (glGetError() != GL_NO_ERROR)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				printf("Error occurred while generating index buffer object for Primitive.\n");
				glDeleteBuffers(1, &indexVBO);
				indexVBO = 0;
				return;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		// Store data
		PrimitiveInstance *pi = instances_.add();
		pi->setVBO(context, vertexVBO, useIndices_ ? indexVBO : 0);
	}
	else
	{
		// Generate display list
		int listId = glGenLists(1);
		glNewList(listId, GL_COMPILE);
		vertexChunk_.sendToGL();
		glEndList();

		// Store data
		PrimitiveInstance *pi = instances_.add();
		pi->setDisplayList(context, listId);
	}
}

// Pop topmost instance on primitive's stack
void Primitive::popInstance(const QGLContext *context)
{
	// Does this primitive use instances?
	if (!useInstances_) return;
	PrimitiveInstance *pi = instances_.last();
	if (pi != NULL)
	{
		if (pi->context() == context)
		{
			// Vertex buffer object or plain old display list?
			if (pi->type() == PrimitiveInstance::VBOInstance)
			{
				GLuint bufid  = pi->vboVertexObject();
				if (bufid != 0) glDeleteBuffers(1, &bufid);
				if (useIndices_)
				{
					bufid = pi->vboIndexObject();
					if (bufid != 0) glDeleteBuffers(1, &bufid);
				}
			}
			else glDeleteLists(pi->listObject(),1);
		}
	}
	instances_.removeLast();
}

// Send to OpenGL (i.e. render)
void Primitive::sendToGL()
{
	// If no vertices are defined, nothing to do...
	if (vertexChunk_.nDefinedVertices() == 0) return;

	// Check if using instances...
	if (useInstances_)
	{
		// Grab topmost instance
		PrimitiveInstance *pi = instances_.last();
		if (pi == NULL) printf("Internal Error: No instance on stack in primitive %p.\n", this);
		else if (pi->type() == PrimitiveInstance::VBOInstance)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			if (useIndices_) glEnableClientState(GL_INDEX_ARRAY);
			else glDisableClientState(GL_INDEX_ARRAY);

			// Bind VBO and index buffer (if using it)
			glBindBuffer(GL_ARRAY_BUFFER, pi->vboVertexObject());
			if (useIndices_) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pi->vboIndexObject());

			glInterleavedArrays(colouredVertexData_ ? GL_C4F_N3F_V3F : GL_N3F_V3F, 0, NULL);
			if (useIndices_) glDrawElements(type_, vertexChunk_.nDefinedIndices(), GL_UNSIGNED_INT, 0);
			else glDrawArrays(type_, 0, vertexChunk_.nDefinedVertices());

			// Revert to normal operation - pass 0 as VBO index
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			if (useIndices_) glDisableClientState(GL_INDEX_ARRAY);
		}
		else glCallList(pi->listObject());
	}
	else vertexChunk_.sendToGL();
}

