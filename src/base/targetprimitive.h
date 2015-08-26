/*
	*** Target Primitive
	*** src/base/targetprimitive.h
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

#ifndef UCHROMA_TARGETPRIMITIVE_H
#define UCHROMA_TARGETPRIMITIVE_H

#include "render/primitivelist.h"

// Forward Declarations
class Collection;
class Axes;

// TargetPrimitive
class TargetPrimitive : public ListItem<TargetPrimitive>
{
	public:
	// Constructor / Destructor
	TargetPrimitive();
	~TargetPrimitive();


	/*
	 * Source Collection
	 */
	private:
	// Source collection
	Collection* collection_;

	public:
	// Set target collection
	void setCollection(Collection* collection);
	// Return target collection
	Collection* collection();


	/*
	 * GL
	 */
	private:
	// Primitive for display of target collection (if necessary)
	PrimitiveList primitive_;
	// Collection data version at which primitive was last created
	int primitiveDataUsedAt_;
	// Collection colour version at which primitive was last created
	int primitiveColourUsedAt_;
	// ViewPane axes version at which primitive was last created
	int primitiveAxesUsedAt_;
	// Collection style version at which primitive was last created
	int primitiveStyleUsedAt_;

	public:
	// Update primitive for target collection, returning if data was changed
	void updateAndSendPrimitive(const Axes& axes, bool forceUpdate, bool pushAndPop, const QOpenGLContext* context);
	// Send primitive to GL
	void sendToGL();
};

#endif
