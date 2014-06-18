/*
	*** Primitive Vertex/Index Generation Functions
	*** src/render/viewer_primitive_vertex.cpp
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

#include "gui/viewer_primitive.h"
#include "math/constants.h"
#include "templates/vector3.h"
#include <math.h>

// Define next vertex and normal
GLuint Primitive::defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz)
{
	return vertexChunk_.defineVertex(x,y,z,nx,ny,nz);
}

// Define next vertex and normal (as Vec3<double>)
GLuint Primitive::defineVertex(Vec3<double> vertex, Vec3<double> normal)
{
	return defineVertex(vertex.x, vertex.y, vertex.z, normal.x, normal.y, normal.z);
}

// Define next vertex and normal with colour
GLuint Primitive::defineVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	return vertexChunk_.defineVertex(x,y,z,nx,ny,nz,r,g,b,a);
}

// Define next vertex and normal with colour (as array)
GLuint Primitive::defineVertex(GLfloat x, GLfloat y, GLfloat z, Vec3<double>& normal, Vec4<GLfloat>& colour)
{
	return vertexChunk_.defineVertex(x,y,z,normal.x,normal.y,normal.z,colour.x,colour.y,colour.z,colour.w);
}

// Define next vertex, normal, and colour (as Vec3<double>s and array)
GLuint Primitive::defineVertex(Vec3<double>& v, Vec3<double>& u, Vec4<GLfloat>& colour)
{
	return vertexChunk_.defineVertex(v.x,v.y,v.z,u.x,u.y,u.z,colour.x,colour.y,colour.z,colour.w);
}

// Define next index triple
void Primitive::defineIndices(GLuint a, GLuint b, GLuint c)
{
	vertexChunk_.defineIndices(a, b, c);
}