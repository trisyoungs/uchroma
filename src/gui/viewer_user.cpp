/*
	*** Scene Rendering Functions (User)
	*** src/gui/viewer_user.cpp
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

#include "gui/viewer.uih"
#include "render/surface.h"
#include "base/collection.h"
#include "base/viewpane.h"

// Create/update surface primitive
bool Viewer::updateSurfacePrimitive(Collection* collection, bool forceUpdate)
{
	// Check for valid collection
	if (!collection) return false;

	// Check whether the primitive for this collection needs updating
	if (collection->displayPrimitivesValid() && collection->colourScaleValid() && (!forceUpdate)) return false;

	// Pop old primitive instances and adjust primitive settings
	collection->displayPrimitives().popInstance(context());
	collection->displayPrimitives().forgetAll();
	
	// Need a valid set of axes in order to construct the surface
	if (!collection->displayPane()) return false;

	// Recreate primitive depending on current style
	double yStretch = collection->displayPane()->axes().stretch(1);
	bool yLogarithmic = collection->displayPane()->axes().logarithmic(1);
	switch (collection->displayStyle())
	{
		case (Collection::LineXYStyle):
			Surface::constructLineXY(collection->displayPrimitives(), yStretch, yLogarithmic, collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
		case (Collection::LineZYStyle):
			Surface::constructLineZY(collection->displayPrimitives(), yStretch, yLogarithmic, collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
		case (Collection::GridStyle):
			Surface::constructGrid(collection->displayPrimitives(), yStretch, yLogarithmic, collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
		case (Collection::SurfaceStyle):
			Surface::constructFull(collection->displayPrimitives(), yStretch, yLogarithmic, collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
	}

	// Push a new instance to create the new display list / vertex array
	collection->displayPrimitives().pushInstance(context(), extensionsStack_.last());
	collection->setDisplayPrimitiveValid();

	return true;
}

// Add supplied surface primitive to list
void Viewer::addCollectionPrimitive(Primitive* primitive)
{
	primitiveList_.add(primitive);
}

// Remove surface primitive from primitive list
void Viewer::removeCollectionPrimitive(Primitive* primitive)
{
	primitiveList_.remove(primitive);
}
