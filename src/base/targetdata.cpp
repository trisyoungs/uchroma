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
#include <X11/Xlib.h>

// Constructor
TargetData::TargetData() : ListItem<TargetData>()
{
	parent_ = NULL;
	collection_ = NULL;
	generatedData_ = new Collection*[TargetData::nGeneratedDataTypes];
	for (int n=0; n<TargetData::nGeneratedDataTypes; ++n) generatedData_[n] = NULL;
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
 * Target Collection and Derived Data
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

/*
 * Display Primitives
 */

// Return first display primitive
TargetPrimitive* TargetData::displayPrimitives()
{
	return displayPrimitives_.first();
}