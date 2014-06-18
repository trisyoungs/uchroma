/*
	*** Rendering PrimitiveList List
	*** src/gui/viewer_primitivelist.cpp
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

#include "gui/viewer_primitivelist.h"
#include "gui/viewer.uih"

/*
 * PrimitiveList
 */

// Constructor
PrimitiveList::PrimitiveList()
{
	viewer_ = NULL;
}

// Destructor
PrimitiveList::~PrimitiveList()
{
	// Remove primitives from Viewer reflist
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) viewer_->removeCollectionPrimitive(prim);
}

// Clear all existing primitives
void PrimitiveList::clear()
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->clear();
}

// Forget all data, leaving arrays intact
void PrimitiveList::forgetAll()
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->forgetAll();
}

// Resize list so it is large enough to accomodate specified number of Primitives
void PrimitiveList::resize(int newSize, bool allowShrink)
{
	// Add enough primitives to match the new size
	while (primitives_.nItems() < newSize)
	{
		Primitive* newPrim = primitives_.add();
		newPrim->setColourData(true);
		viewer_->addCollectionPrimitive(newPrim);
	}

	// Shrink list to new size (if allowed)
	if (allowShrink)
	{
		// Manage primitive list, reusing where possible...
		while (primitives_.nItems() > newSize)
		{
			viewer_->removeCollectionPrimitive(primitives_.last());
			primitives_.removeLast();
		}
	}
}

// Return total number of defined vertices
int PrimitiveList::nDefinedVertices()
{
	int totalVertices = 0;
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) totalVertices += prim->nDefinedVertices();
	return totalVertices;
}

// Return total number of defined indices
int PrimitiveList::nDefinedIndices()
{
	int totalIndices = 0;
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) totalIndices += prim->nDefinedIndices();
	return totalIndices;
}

// Set viewer
void PrimitiveList::setViewer(Viewer* viewer)
{
	viewer_ = viewer;
}

// Flag that primitive should contain colour data information for each vertex
void PrimitiveList::setColourData(bool b)
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->setColourData(b);
}


// Set GL drawing primitive type
void PrimitiveList::setType(GLenum type)
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->setType(type);
}

// Push instance layer
void PrimitiveList::pushInstance(const QGLContext *context)
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->pushInstance(context);
}

// Pop topmost instance layer
void PrimitiveList::popInstance(const QGLContext *context)
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->popInstance(context);
}

// Send to OpenGL (i.e. render)
void PrimitiveList::sendToGL()
{
	for (Primitive* prim = primitives_.first(); prim != NULL; prim = prim->next) prim->sendToGL();
}

/*
 * Operators
 */

// Element access operator
Primitive *PrimitiveList::operator[](int index)
{
	return primitives_[index];
}
