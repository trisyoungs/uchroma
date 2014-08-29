/*
	*** DataSpaceRange
	*** src/base/dataspacerange.h
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

#ifndef UCHROMA_DATASPACERANGE_H
#define UCHROMA_DATASPACERANGE_H

#include "base/dataspacedata.h"
#include "templates/list.h"

// Forward Declarations
class Collection;
class DataSpace;
class Expression;
class Variable;

/*
 * DataSpaceRange
 */
class DataSpaceRange : public ListItem<DataSpaceRange>
{
	public:
	// Constructor / Destructor
	DataSpaceRange(DataSpace& parent);
	~DataSpaceRange();

	private:
	// Parent
	DataSpace& parent_;


	/* 
	 * Target Data
	 */
	private:
	// Indices of first and last DisplayDataSet in this target
	int displayDataSetStart_, displayDataSetEnd_;
	// Number of sequential DisplayDataSet in this target
	int nDataSets_;
	// First and last abscissa indices in this target
	int abscissaStart_, abscissaEnd_;
	// Number of sequential abscissa values in this target
	int nPoints_;

	public:
	// Set target information
	void set(Collection* collection, int abscissaFirst, int abscissaLast, int firstDataSet, int lastDataSet);
	// Return index of first DisplayDataSet to be fit
	int displayDataSetStart();
	// Return index of last DisplayDataSet to be fit
	int displayDataSetEnd();
	// Return number of sequential DisplayDataSets in this target
	int nDataSets();
	// Return first abscissa index in this target
	int abscissaStart();
	// Return last abscissa index in this target
	int abscissaEnd();
	// Return number of sequential abscissa values in this target
	int nPoints();
	// Return starting x value of range
	double xStart();
	// Return final x value of range
	double xEnd();
	// Return starting z value of range
	double zStart();
	// Return final z value of range
	double zEnd();


	/*
	 * Associated Data
	 */
	private:
	// Associated values for target space
	List<DataSpaceData> values_;

	public:
	// Calculate values from specified equation
	bool calculateValues(Expression& equation, Variable* xVariable, Variable* zVariable);
	// Return sos error between stored and referenced values
	double sosError();
	// Copy values to specified Collection
	void copyCalculatedValues(Collection* target);
};

#endif
