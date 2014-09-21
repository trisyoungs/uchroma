/*
	*** Target Data
	*** src/base/targetdata.cpp
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

#include "base/targetdata.h"
#include "base/collection.h"
#include "base/viewpane.h"
#include "render/surface.h"

// Constructor
TargetData::TargetData() : ListItem<TargetData>()
{
	parent_ = NULL;
	collection_ = NULL;
	primitiveDataUsedAt_ = -1;
	primitiveColourUsedAt_ = -1;
	primitiveStyleUsedAt_ = -1;
	primitiveAxesUsedAt_ = -1;
}

// Destructor
TargetData::~TargetData()
{
}

/*
 * Parent
 */

// Set parent ViewPane
void TargetData::setParent(ViewPane* parent)
{
	parent_ = parent;
}

/*
 * Target Collection / Primitive
 */

// Set pointer to target collection
void TargetData::setCollection(Collection* collection)
{
	collection_ = collection;
}

// Return pointer to target collection
Collection* TargetData::collection()
{
	return collection_;
}

// Return primitive for display
PrimitiveList& TargetData::primitive()
{
	return primitive_;
}

/*
 * GL
 */

// Update primitive
PrimitiveList& TargetData::updatePrimitive(const QGLContext* context, GLExtensions* extensions, const Axes& axes, bool forcePrimitiveUpdate, bool dontPopInstance)
{
	// Check collection validity
	if (!Collection::objectValid(collection_, "collection in TargetData::updatePrimitive")) return primitive_;

	// Check whether the primitive for this collection needs updating
	bool upToDate = true;
	if (forcePrimitiveUpdate) upToDate = false;
	else if (primitiveAxesUsedAt_ != axes.displayVersion()) upToDate = false;
	else if (primitiveColourUsedAt_ != collection_->colourVersion()) upToDate = false;
	else if (primitiveDataUsedAt_ != collection_->dataVersion()) upToDate = false;
	else if (primitiveStyleUsedAt_ != collection_->styleVersion()) upToDate = false;

	// Pop old primitive instance (unless flagged not to)
	if (!dontPopInstance) primitive_.popInstance(context);

	// Recreate primitive depending on current style
	switch (collection_->displayStyle())
	{
		case (Collection::LineXYStyle):
			Surface::constructLineXY(primitive_, axes, collection_->displayAbscissa(), collection_->displayData(), collection_->colourScale());
			break;
		case (Collection::LineZYStyle):
			Surface::constructLineZY(primitive_, axes, collection_->displayAbscissa(), collection_->displayData(), collection_->colourScale());
			break;
		case (Collection::GridStyle):
			Surface::constructGrid(primitive_, axes, collection_->displayAbscissa(), collection_->displayData(), collection_->colourScale());
			break;
		case (Collection::SurfaceStyle):
			Surface::constructFull(primitive_, axes, collection_->displayAbscissa(), collection_->displayData(), collection_->colourScale());
			break;
	}

	// Push a new instance to create the new display list / vertex array
	primitive_.pushInstance(context, extensions);

	// Store version points for the up-to-date primitive
	primitiveAxesUsedAt_ = axes.displayVersion();
	primitiveColourUsedAt_ = collection_->colourVersion();
	primitiveDataUsedAt_ = collection_->dataVersion();
	primitiveStyleUsedAt_ = collection_->styleVersion();
}

// Send collection data to GL, including any associated fit and extracted data
void TargetData::sendToGL()
{
	// Check collection validity
	if (!Collection::objectValid(collection_, "collection in TargetData::updatePrimitive")) return;

	// If this collection is not visible return now
	if (!collection_->visible()) return;

	if (collection_->displayStyle() == Collection::SurfaceStyle)
	{
		glEnable(GL_LIGHTING);
		glDisable(GL_LINE_SMOOTH);
	}
	else
	{
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(collection_->displayLineWidth());
		glDisable(GL_LIGHTING);
	}

	// Send Primitives to display
	primitive_.sendToGL();
}
