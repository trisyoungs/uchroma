/*
	*** Target Data
	*** src/base/targetdata.h
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

#ifndef UCHROMA_TARGETDATA_H
#define UCHROMA_TARGETDATA_H

#include "templates/reflist.h"

// Forward Declarations
class Collection;
class ViewPane;

// TargetData
class TargetData
{
	public:
	// Constructor / Destructor
	TargetData();
	~TargetData();
	// Data Type
	enum TargetDataType { SliceData, nTargetDataTypes };


	/*
	 * Parent
	 */
	private:
	// Parent ViewPane
	ViewPane* parent_;

	public:
	// Set parent ViewPane
	void setParent(ViewPane* parent);


	/*
	 * Collection Data
	 */
	private:
	// Array of potential target data
	Collection** collectionData_;

	public:
	// Add new collection data
	Collection* addCollectionData(TargetData::TargetDataType type);
	// Return specified collection data type (if it exists)
	Collection* collectionData(TargetData::TargetDataType type);
};

#endif
