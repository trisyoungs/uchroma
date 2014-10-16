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
TargetData::TargetData(ViewPane& parent) : ListItem<TargetData>(), parent_(parent)
{
	collection_ = NULL;
	generatedData_ = new Collection*[TargetData::nGeneratedDataTypes];
	for (int n=0; n<TargetData::nGeneratedDataTypes; ++n) generatedData_[n] = NULL;
}

// Destructor
TargetData::~TargetData()
{
	clear();

	delete[] generatedData_;
}

/*
 * Target Collection and Derived Data
 */

// Clear and nullify all data / pointers
void TargetData::clear()
{
	// Clear display list first....
	displayPrimitives_.clear();

	// Now remove any generated data
	for (int n=0; n<TargetData::nGeneratedDataTypes; ++n)
	{
		if (generatedData_[n]) delete generatedData_[n];
		generatedData_[n] = NULL;
	}
}

// Initialise with specified collection
void TargetData::initialise(Collection* collection)
{
	// Clear any old data
	clear();

	// Set target collection
	collection_ = collection;

	// Check collection validity
	if (!Collection::objectValid(collection_, "collection in TargetData::initialise()")) return;

	// Set up additional data based on the parent ViewPane's type
	switch (parent_.role())
	{
		// Standard view pane - add a primitive for the target collection
		case (ViewPane::StandardRole):
			addDisplayPrimitive(collection);
			break;
		case (ViewPane::SliceMonitorRole):
			addDisplayPrimitive(collection->currentSlice());
			break;
		case (ViewPane::ExtractorRole):
			addDisplayPrimitive(collection);
			break;
		default:
			msg.print("Internal Error: ViewPane role '%s' not accounted for in TargetData::initialise().\n", ViewPane::paneRole(parent_.role()));
			break;
	}
}

// Return pointer to target collection
Collection* TargetData::collection()
{
	return collection_;
}

/*
 * Display Data
 */

// Add target primitive for specified collection
void TargetData::addDisplayPrimitive(Collection* collection)
{
	TargetPrimitive* primitive = displayPrimitives_.add();
	primitive->setCollection(collection);
}

// Return first display primitive
TargetPrimitive* TargetData::displayPrimitives()
{
	return displayPrimitives_.first();
}