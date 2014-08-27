/*
	*** DataSpace
	*** src/base/dataspace.h
	Copyright T. Youngs 2012-2014.

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

#ifndef UCHROMA_DATASPACE_H
#define UCHROMA_DATASPACE_H

#include "base/dataspacerange.h"
#include "templates/list.h"

// Forward Declarations
class Collection;

/*
 * DataSpace
 */
class DataSpace
{
	public:
	// Constructor
	DataSpace();
	// Destructor
	~DataSpace();


	/*
	 * Data
	 */
	private:
	// Source collection
	Collection* sourceCollection_;
	// Range of datasets spanned by this instance
	int displayDataSetStart_, displayDataSetEnd_;
	// Total number of sequential DisplayDataSet covered
	int nDataSets_;
	// Range of datasets spanned by this instance
	int abscissaStart_, abscissaEnd_;
	// Total number of sequential abscissa values covered
	int nPoints_;
	// List of individual fitting targets / slices / ranges
	ParentList<DataSpaceRange,DataSpace> ranges_;

	public:
	// Initialise ranges
	bool initialise(Collection* sourceCollection, int xIndexMin, int xIndexMax, int zIndexMin, int zIndexMax, bool orthogonal, bool global);
	// Return source collection
	Collection* sourceCollection();
	// Return range list
	DataSpaceRange* ranges();
	// Copy calculated y data to destination collection specified
	void copy(Collection* destinationCollection);
};

#endif