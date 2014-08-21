/*
	*** View Pane
	*** src/base/viewpane.cpp
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
#include "collection.h"
#include "viewpane.h"

// Constructor
TargetData::TargetData()
{
	parent_ = NULL;
	collectionData_ = new Collection*[TargetData::nTargetDataTypes];
	for (int n=0; n<TargetData::nTargetDataTypes; ++n) collectionData_[n] = NULL;
}

// Destructor
TargetData::~TargetData()
{
	for (int n=0; n<TargetData::nTargetDataTypes; ++n) if (collectionData_[n]) delete collectionData_[n];
	delete[] collectionData_;
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
 * Collection Data
 */

// Add new collection data
Collection* TargetData::addCollectionData(TargetData::TargetDataType type)
{
	// Check if the data already exists
	Collection* data = collectionData(type);
	if (data) data->clearDataSets();
	else
	{
		collectionData_[type] = new Collection;
		parent_->addCollection(collectionData_[type]);
		data = collectionData_[type];
	}

	return data;
}

// Check if specified collection data type already exists
Collection* TargetData::collectionData(TargetData::TargetDataType type)
{
	return collectionData_[type];
}
