/*
	*** DataSpaceData
	*** src/baes/dataspacedata.h
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

#ifndef UCHROMA_DATASPACEDATA_H
#define UCHROMA_DATASPACEDATA_H

#include "base/displaydataset.h"
#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
/* none */

/*
 * DataSpaceData
 */
class DataSpaceData : public ListItem<DataSpaceData>
{
	private:
	// Array of X values
	Array<double> x_;
	// Array of reference Y values
	Array<double> yReference_;
	// Array of calculated Y values
	Array<double> yCalculated_;
	// Array of Y value types
	Array<DisplayDataSet::DataPointType> yType_;
	// Z value
	double z_;

	public:
	// Add point to arrays
	void addPoint(double x, double yRef, DisplayDataSet::DataPointType yType);
	// Return X array
	const Array<double> x();
	// Return reference Y array
	const Array<double> yReference();
	// Set specified calculated Y element
	void setCalculatedY(int index, double value);
	// Return calculated Y array
	const Array<double> yCalculated();
	// Return Y type array
	const Array<DisplayDataSet::DataPointType> yType();
	// Set z value
	void setZ(double z);
	// Return z value
	double z() const;
};

#endif
