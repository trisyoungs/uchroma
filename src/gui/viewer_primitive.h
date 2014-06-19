/*
	*** Rendering Primitive
	*** src/gui/viewer_primitive.h
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

#ifndef UCHROMA_PRIMITIVE_H
#define UCHROMA_PRIMITIVE_H

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include "glext.h"
#endif
#include "templates/list.h"
#include "gui/viewer_vertexchunk.h"
#include "math/matrix.h"
#include "base/dnchar.h"

// Forward Declarations
class QGLContext;

// Primitive Instance
class PrimitiveInstance : public ListItem<PrimitiveInstance>
{
	public:
	// Constructor
	PrimitiveInstance();
	// Instance Type
	enum InstanceType { NoInstances, ListInstance, VBOInstance };
	
	private:
	// Context to which primitive instance is associated
	const QGLContext *context_;
	// Type of instance
	InstanceType type_;
	// List ID of instance (if using display lists)
	GLuint listObject_;
	// VBO ID of vertex array (if using VBOs)
	GLuint vboVertexObject_;
	// VBO ID of index array (if using indexed VBOs)
	GLuint vboIndexObject_;
	
	public:
	// Set display list data
	void setDisplayList(const QGLContext *context, GLuint listObject);
	// Set vbo object data
	void setVBO(const QGLContext *context, GLuint vertexObject, GLuint indexObject);
	// Return context to which primitive instance is associated
	const QGLContext *context();
	// Return type of instance
	InstanceType type();
	// Return display list object for instance
	GLuint listObject();
	// Return VBO ID of vertex array for instance
	GLuint vboVertexObject();
	// Return VBO ID of index array for instance
	GLuint vboIndexObject();
};

// Rendering Primitive
class Primitive : public ListItem<Primitive>
{
	public:
	// Constructor / Destructor
	Primitive();
	~Primitive();

	/*
	 * OpenGL Function Pointers (VBO)
	 */
	static PFNGLGENBUFFERSPROC glGenBuffers;
	static PFNGLBINDBUFFERPROC glBindBuffer;
	static PFNGLBUFFERDATAPROC glBufferData;
	static PFNGLBUFFERSUBDATAPROC glBufferSubData;
	static PFNGLDELETEBUFFERSPROC glDeleteBuffers;


	/*
	// Data
	*/
	private:
	// Vertex chunk for this primitive
	VertexChunk vertexChunk_;
	// Whether vertexData_ array also contains colour information
	bool colouredVertexData_;
	// GL object drawing method
	GLenum type_;
	// Default (i.e. global) instance type to use
	static const PrimitiveInstance::InstanceType defaultInstanceType_ = PrimitiveInstance::VBOInstance;
	// Stack of OpenGL VBO or display list IDs and the contexts in which they were created
	List<PrimitiveInstance> instances_;
	// Flag stating whether or not instances should be used for this primitive
	bool useInstances_;
	// Flag stating whether indexed vertex drawing is to be used
	bool useIndices_;
	// Name of primitive (for easier bug-tracking)
	Dnchar name_;

	public:
	// Initialise primitive storage
	void initialise(int maxVertices, int maxIndices, GLenum type, bool colourData);
	// Forget all data, leaving arrays intact
	void forgetAll();
	// Return number of vertices currently defined in primitive
	int nDefinedVertices();
	// Return number of indices currently defined in primitive
	int nDefinedIndices();
	// Return vertex chunk
	const VertexChunk& vertexChunk();
	// Return whether vertex data contains colour information
	bool colouredVertexData();
	// Flag that this primitive should not use instances (rendering will use vertex arrays)
	void setNoInstances();
	// Set whether indexed vertex drawing is to be used
	void setUseIndices(bool b);
	// Push instance layer from current vertex chunk list
	void pushInstance(const QGLContext *context);
	// Pop topmost instance layer
	void popInstance(const QGLContext *context);
	// Set name of primitive
	void setName(const char *s);
	// Return name of primitive
	const char *name();
	// Send to OpenGL (i.e. render)
	void sendToGL();
	
	
	/*
	// Vertex / Index Generation
	*/
	public:
	// Define next vertex and normal
	GLuint defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz);
	// Define next vertex and normal (as Vec3<double>)
	GLuint defineVertex(Vec3<double> vertex, Vec3<double> normal);
	// Define next vertex, normal, and colour
	GLuint defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	// Define next vertex, normal, and colour
	GLuint defineVertex(GLfloat x, GLfloat y, GLfloat z, Vec3<double>& normal, Vec4<GLfloat>& colour);
	// Define next vertex, normal, and colour (as Vec3<double>s and array)
	GLuint defineVertex(Vec3<double> &v, Vec3<double> &u, Vec4<GLfloat> &colour);
	// Define next index double
		bool defineIndices(GLuint a, GLuint b);
	// Define next index triple
		bool defineIndices(GLuint a, GLuint b, GLuint c);
};

#endif
