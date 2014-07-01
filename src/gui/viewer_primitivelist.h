/*
	*** Rendering Primitive List
	*** src/gui/viewer_primitivelist.h
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

#ifndef UCHROMA_PRIMITIVELIST_H
#define UCHROMA_PRIMITIVELIST_H

#include "gui/viewer_primitive.h"
#include "templates/list.h"

// Forward Declarations
class QGLContext;
class Viewer;

// Rendering Primitive List
class PrimitiveList
{
	public:
	// Constructor / Destructor
	PrimitiveList();
	~PrimitiveList();


	/*
	// Data
	*/
	private:
	// List of Primitives owned and managed by this list
	List<Primitive> primitives_;
	// Viewer in which these primitives are to be displayed
	Viewer* viewer_;

	public:
	// Set Viewer in which these primitives are to be displayed
	void setViewer(Viewer* viewer);
	// Return viewer in which these primitives are to be displayed
	Viewer* viewer();
	// Clear existing data
	void clear();
	// Forget all data, leaving arrays intact
	void forgetAll();
	// Reinitialise list so it is large enough to accomodate specified number of Primitives
	void reinitialise(int newSize, bool allowShrink, int maxVertices, int maxIndices, GLenum type, bool colourData);
	// Add a new primitive to the end of the list
	Primitive* addPrimitive(int maxVertices, int maxIndices, GLenum type, bool colourData);
	// Return total number of defined vertices
	int nDefinedVertices();
	// Return total number of defined indices
	int nDefinedIndices();
	// Push instance layer
	void pushInstance(const QGLContext *context);
	// Pop topmost instance layer
	void popInstance(const QGLContext *context);
	// Send to OpenGL (i.e. render)
	void sendToGL();


	/*
	 * Operators
	 */
	public:
	// Element access operator
	Primitive *operator[](int index);
};

#endif
