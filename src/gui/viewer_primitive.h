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
#include "gui/viewer_indexchunk.h"
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
	// VBO function pointers (Windows only)
	#ifdef _WIN32
	static PFNGLGENBUFFERSPROC glGenBuffers;
	static PFNGLBINDBUFFERPROC glBindBuffer;
	static PFNGLBUFFERDATAPROC glBufferData;
	static PFNGLBUFFERSUBDATAPROC glBufferSubData;
	static PFNGLDELETEBUFFERSPROC glDeleteBuffers;
	#endif


	/*
	// Data
	*/
	private:
	// List of vertices in primitive
	List<VertexChunk> vertexChunks_;
	// Current vertex chunk
	VertexChunk* currentVertexChunk_;
	// Whether vertexData_ array also contains colour information
	bool colouredVertexData_;
	// Number of vertices that have been defined
	int nDefinedVertices_;
	// Index array, referring to vertexChunk list
	List<IndexChunk> indexChunks_;
	// Current index chunk
	IndexChunk* currentIndexChunk_;
	// Number of defined indices
	int nDefinedIndices_;
	// GL object drawing method
	GLenum type_;
	// Default (i.e. global) instance type to use
	static const PrimitiveInstance::InstanceType defaultInstanceType_ = PrimitiveInstance::ListInstance;
	// Stack of OpenGL VBO or display list IDs and the contexts in which they were created
	List<PrimitiveInstance> instances_;
	// Flag stating whether or not instances should be used for this primitive
	bool useInstances_;
	// Flag stating whether indexed vertex drawing is to be used
	bool useIndices_;
	// Name of primitive (for easier bug-tracking)
	Dnchar name_;

	public:
	// Clear existing data (including deleting arrays)
	void clear();
	// Forget all data, leaving arrays intact
	void forgetAll();
	// Return number of vertices currently defined in primitive
	int nDefinedVertices();
	// Return number of indices currently defined in primitive
	int nDefinedIndices();
	// Set GL drawing primitive type
	void setType(GLenum type);
	// Return vertex array
	VertexChunk *vertexChunks();
	// Flag whether primitive should contain colour data information for each vertex
	void setColourData(bool b);
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
	// Vertex Generation
	*/
	public:
	// Define next vertex and normal
	void defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, bool calcCentroid);
	// Define next vertex and normal (as Vec3<double>)
	void defineVertex(Vec3<double> vertex, Vec3<double> normal, bool calcCentroid);
	// Define next vertex, normal, and colour
	void defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b, GLfloat a, bool calcCentroid);
	// Define next vertex, normal, and colour
	void defineVertex(GLfloat x, GLfloat y, GLfloat z, Vec3<double>& normal, Vec4<GLfloat>& colour, bool calcCentroid);
	// Define next vertex, normal, and colour (as Vec3<double>s and array)
	void defineVertex(Vec3<double> &v, Vec3<double> &u, Vec4<GLfloat> &colour, bool calcCentroid);
	// Define triangle fromn supplied array data, unique colour per vertex
	void defineTriangle(GLfloat *vertices, GLfloat *normals, GLfloat *colour);
	// Define triangle with single colour per vertex
	void defineTriangleSingleColour(GLfloat *vertices, GLfloat *normals, GLfloat *colour);
	// Plot simple line between specified coordinates
	void plotLine(Vec3<double> p1, Vec3<double> p2);
	// Plot vertices of sphere with specified radius and quality
	void plotSphere(double radius, int nstacks, int nslices);
	// Plot cylinder vertices from origin {ox,oy,oz}, following vector {vx,vy,vz}, for 'length', with radii and quality specified
	void plotCylinder(GLfloat ox, GLfloat oy, GLfloat oz, GLfloat vx, GLfloat vy, GLfloat vz, double startradius, double endradius, int nstacks, int nslices, bool capStart = false, bool capEnd = false);
	// Plot tube ring of specified radius and tube width
	void plotRing(double radius, double width, int nstacks, int nslices, int nsegments, bool segmented = false);
	// Plot circle of specified radius
	void plotCircle(double radius, int nstacks, int nsegments, bool segmented = false);
	// Create vertices of cross with specified width
	void plotCross(double halfWidth, Matrix &transform, GLfloat colour[4]);
	// Plot solid cube of specified size at specified origin, and with sides subdivided into triangles ( ntriangles = 2*nsubs )
	void plotCube(double size, int nsubs, double ox, double oy, double oz);
	// Plot solid orthorhomboid of specified size at specified origin, and with sides subdivided into triangles ( ntriangles = 2*nsubs )
	void plotOrthorhomboid(double sizex, double sizey, double sizez, int nsubs, double ox, double oy, double oz);

	/*
	 * Index Generation
	 */
	public:
	// Define next index triple
	void defineIndices(GLuint a, GLuint b, GLuint c);


	/*
	// Primitive Generation
	*/
	public:
	// Create wireframe cube centred at zero
	void createWireCube(double size);
	// Create wireframe, crossed, cube centred at zero
	void createCrossedCube(double size);
	// Create solid cube of specified size at specified origin, and with sides subdivided into triangles ( ntriangles = 2*nsubs )
	void createCube(double size, int nsubs, double ox, double oy, double oz);
};

#endif
