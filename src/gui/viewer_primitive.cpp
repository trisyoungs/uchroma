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
	currentVertexChunk_ = NULL;
	currentIndexChunk_ = NULL;
	colouredVertexData_ = false;
	type_ = GL_TRIANGLES;
	nDefinedVertices_ = 0;
	nDefinedIndices_ = 0;
	useInstances_ = true;
	useIndices_ = false;
	name_ = "<UnnamedPrimitive>";
}

// Destructor
Primitive::~Primitive()
{
	clear();
}

// Flag that primitive should contain colour data information for each vertex
void Primitive::setColourData(bool b)
{
	colouredVertexData_ = b;
}

// Clear existing data
void Primitive::clear()
{
	vertexChunks_.clear();
	currentVertexChunk_ = NULL;
	indexChunks_.clear();
	currentIndexChunk_ = NULL;
	nDefinedVertices_ = 0;
}

// Forget all data, leaving arrays intact
void Primitive::forgetAll()
{
	for (VertexChunk *v = vertexChunks_.first(); v != NULL; v = v->next) v->forgetAll();
	currentVertexChunk_ = vertexChunks_.first();
	for (IndexChunk *i = indexChunks_.first(); i != NULL; i = i->next) i->forgetAll();
	currentIndexChunk_ = indexChunks_.first();
	nDefinedVertices_ = 0;
}

// Return number of vertices currently defined in primitive
int Primitive::nDefinedVertices()
{
	return nDefinedVertices_;
}

// Return number of indices currently defined in primitive
int Primitive::nDefinedIndices()
{
	return nDefinedIndices_;
}

// Set GL drawing primitive type
void Primitive::setType(GLenum type)
{
	type_ = type;
	clear();
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

/*
// Primitive Generation
*/

// Create wireframe cube centred at zero
void Primitive::createWireCube(double size)
{
	// Clear existing data first (if it exists)
	type_ = GL_LINES;
	forgetAll();
	
	size = 0.5*size;
	int i, j;
	GLfloat r[3];
	// Set initial corner
	r[0] = -size;
	r[1] = -size;
	r[2] = -size;
	for (i=0; i<4; ++i)
	{
		// Swap signs to generate new corner if necessary
		if (i>0)
		{
			r[1] = -r[1];
			if (i == 2) r[2] = -r[2];
			else r[0] = -r[0];
		}
		// Generate lines
		for (j=0; j<3; ++j)
		{
			defineVertex(r[0], r[1], r[2], 1.0, 0.0, 0.0, true);
			defineVertex(j == 0 ? -r[0] : r[0], j == 1 ? -r[1] : r[1], j == 2 ? -r[2] : r[2], 1.0, 0.0, 0.0, true);
		}
	}
}

// Create wireframe, crossed cube centred at zero
void Primitive::createCrossedCube(double size)
{
	// Create wire cube to start with
	createWireCube(size);
	// Add crosses to faces
	int i, j, sign;
	GLfloat r[3];
	for (i=0; i<3; ++i)
	{
		for (sign = 1; sign > -2; sign -=2)
		{
			// Determine single coordinate on positive face from which to determine all others
			for (j = 0; j<3; ++j) r[j] = (j == i ? 0.55*size*sign : 0.4*size);
			defineVertex(r[0], r[1], r[2], 1.0, 0.0, 0.0, true);
			r[(i+1)%3] = -r[(i+1)%3];
			r[(i+2)%3] = -r[(i+2)%3];
			defineVertex(r[0], r[1], r[2], 1.0, 0.0, 0.0, true);
			r[(i+1)%3] = -r[(i+1)%3];
			defineVertex(r[0], r[1], r[2], 1.0, 0.0, 0.0, true);
			r[(i+1)%3] = -r[(i+1)%3];
			r[(i+2)%3] = -r[(i+2)%3];
			defineVertex(r[0], r[1], r[2], 1.0, 0.0, 0.0, true);
		}
	}
}

// Create solid cube of specified size, centred at zero, and with sides subdivided into triangles ( ntriangles = 2*nsubs )
void Primitive::createCube(double size, int nsubs, double ox, double oy, double oz)
{
	// Clear existing data first (if it exists)
	forgetAll();
	plotCube(size, nsubs, ox, oy, oz);
}

// Return first chunk vertex array
VertexChunk *Primitive::vertexChunks()
{
	return vertexChunks_.first();;
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
		if (nDefinedVertices_ < 0)
		{
			printf("Error: No data in Primitive with which to create VBO.\n");
			msg.exit("Primitive::pushInstance");
			return;
		}
		
		// Determine total size of array (in bytes) for VBO
		int vboSize = nDefinedVertices_ * (colouredVertexData_ ? 10 : 6) * sizeof(GLfloat);
		
		// Generate vertex array object
		glGenBuffers(1, &vertexVBO);

		// Bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		
		// Initialise VBO data, but don't copy anything here
		glBufferData(GL_ARRAY_BUFFER, vboSize, NULL, GL_STATIC_DRAW);
		if (glGetError() != GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			printf("Error occurred while generating vertex buffer object for Primitive.\n");
			glDeleteBuffers(1, &vertexVBO);
			vertexVBO = 0;
			return;
		}

	// 	GLfloat *bufdat = (GLfloat*) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	// // 	for (int n=0; n<30; ++n) printf("Buffer data %i is %f\n", n, bufdat[n]);
	// 	glUnmapBuffer(GL_ARRAY_BUFFER);
		
		// Loop over stored VertexChunks and copy data to VBO
		offset = 0;
		int chunksize;
		for (VertexChunk *chunk = vertexChunks_.first(); chunk != NULL; chunk = chunk->next)
		{
			chunksize = chunk->nDefinedVertices()*(colouredVertexData_ ? 10 : 6)*sizeof(GLfloat);
			glBufferSubData(GL_ARRAY_BUFFER, offset, chunksize, chunk->vertexData());
			offset += chunksize;
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Generate index array ovject (if using indices)
		if (useIndices_)
		{
			// Generate vertex array object
			glGenBuffers(1, &indexVBO);

			// Bind VBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
			
			// Initialise VBO data, but don't copy anything here
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, nDefinedIndices_*sizeof(GLuint), NULL, GL_STATIC_DRAW);
			if (glGetError() != GL_NO_ERROR)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				printf("Error occurred while generating index buffer object for Primitive.\n");
				glDeleteBuffers(1, &indexVBO);
				indexVBO = 0;
				return;
			}

			// Loop over stored IndexChunks and copy data to VBO
			offset = 0;
			for (IndexChunk *ichunk = indexChunks_.first(); ichunk != NULL; ichunk = ichunk->next)
			{
				chunksize = ichunk->nDefinedIndices()*sizeof(GLuint);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, chunksize, ichunk->indexData());
				offset += chunksize;
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
		for (VertexChunk *chunk = vertexChunks_.first(); chunk != NULL; chunk = chunk->next) chunk->sendToGL();
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
	if (nDefinedVertices_ == 0) return;

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
			if (useIndices_) glDrawElements(type_, nDefinedIndices_, GL_UNSIGNED_INT, 0);
			else glDrawArrays(type_, 0, nDefinedVertices_);

			// Revert to normal operation - pass 0 as VBO index
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			if (useIndices_) glDisableClientState(GL_INDEX_ARRAY);
		}
		else glCallList(pi->listObject());
	}
	else for (VertexChunk *chunk = vertexChunks_.first(); chunk != NULL; chunk = chunk->next) chunk->sendToGL();
}

