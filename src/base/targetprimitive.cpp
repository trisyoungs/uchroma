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

#include "base/targetprimitive.h"
#include "base/axes.h"
#include "base/collection.h"
#include "render/surface.h"

// Constructor
TargetPrimitive::TargetPrimitive() : ListItem<TargetPrimitive>()
{
	collection_ = NULL;
	primitiveDataUsedAt_ = -1;
	primitiveColourUsedAt_ = -1;
	primitiveStyleUsedAt_ = -1;
	primitiveAxesUsedAt_ = -1;
}

// Destructor
TargetPrimitive::~TargetPrimitive()
{
}

/*
 * Source Collection
 */

// Set pointer to target collection
void TargetPrimitive::setCollection(Collection* collection)
{
	collection_ = collection;
}

// Return pointer to target collection
Collection* TargetPrimitive::collection()
{
	return collection_;
}

/*
 * GL
 */

// Update and send primitive
void TargetPrimitive::updateAndSendPrimitive(const Axes& axes, bool forceUpdate, bool pushAndPop, const QGLContext* context, GLExtensions* extensions)
{
	// Check collection validity
	if (!Collection::objectValid(collection_, "collection in TargetPrimitive::updateAndSendPrimitive")) return;

	// Check whether the primitive for this collection needs updating
	bool upToDate = true;
	if (forceUpdate) upToDate = false;
	else if (primitiveAxesUsedAt_ != axes.displayVersion()) upToDate = false;
	else if (primitiveColourUsedAt_ != collection_->colourVersion()) upToDate = false;
	else if (primitiveDataUsedAt_ != collection_->dataVersion()) upToDate = false;
	else if (primitiveStyleUsedAt_ != collection_->styleVersion()) upToDate = false;

	// If the primitive is out of date, recreate it's data.
	if (!upToDate)
	{
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

		// Pop old primitive instance (unless flagged not to)
		if ((!pushAndPop) && (primitive_.nInstances() != 0)) primitive_.popInstance(context);
	
		// Push a new instance to create the new display list / vertex array
		primitive_.pushInstance(context, extensions);
	}

	// Send primitive
	sendToGL();

	// Pop current instance (if flagged)
	if (pushAndPop) primitive_.popInstance(context);

	// Store version points for the up-to-date primitive
	primitiveAxesUsedAt_ = axes.displayVersion();
	primitiveColourUsedAt_ = collection_->colourVersion();
	primitiveDataUsedAt_ = collection_->dataVersion();
	primitiveStyleUsedAt_ = collection_->styleVersion();

	return;
}

// Send collection data to GL, including any associated fit and extracted data
void TargetPrimitive::sendToGL()
{
	// Check collection validity
	if (!Collection::objectValid(collection_, "collection in TargetPrimitive::updatePrimitive")) return;

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
		collection_->displayLineStyle().apply();
		glDisable(GL_LIGHTING);
	}

	// Send Primitives to display
	primitive_.sendToGL();
}
