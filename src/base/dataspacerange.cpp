/*
	*** DataSpaceRange
	*** src/base/dataspacerange.cpp
	Copyright T. Youngs 2012-2014

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

#include "base/dataspace.h"
#include "base/collection.h"
#include "base/referencevariable.h"
#include "expression/expression.h"
#include "expression/variable.h"

// Constructor
DataSpaceRange::DataSpaceRange(DataSpace& parent) : ListItem<DataSpaceRange>(), parent_(parent)
{
	displayDataSetStart_ = -1;
	displayDataSetEnd_ = -1;
	nDataSets_ = 0;
	abscissaStart_ = -1;
	abscissaEnd_ = -1;
	nPoints_ = 0;
}

// Destructor
DataSpaceRange::~DataSpaceRange()
{
}

/* 
 * Target Data
 */

// Set target information
void DataSpaceRange::set(Collection* collection, int abscissaFirst, int abscissaLast, int firstDataSet, int lastDataSet, bool referenceDataOnly)
{
	displayDataSetStart_ = firstDataSet;
	displayDataSetEnd_ = lastDataSet;
	nDataSets_ = (displayDataSetEnd_ - displayDataSetStart_) + 1;
	abscissaStart_ = abscissaFirst;
	abscissaEnd_ = abscissaLast;
	nPoints_ = (abscissaEnd_ - abscissaStart_) + 1;

	// Setup data arrays
	values_.clear();
	const Array<double>& abscissa = collection->displayAbscissa();
	DisplayDataSet** dataSets = collection->displayData().array();
	for (int n=0; n<nDataSets_; ++n)
	{
		DataSpaceData* data = values_.add();
		const Array<double>& y = dataSets[n+displayDataSetStart_]->y();
		const Array<DisplayDataSet::DataPointType>& yType = dataSets[n+displayDataSetStart_]->yType();
		if (referenceDataOnly) for (int i=0; i<nPoints_; ++i) data->addPoint(abscissa.value(i+abscissaStart_), y.value(i+abscissaStart_));
		else for (int i=0; i<nPoints_; ++i) data->addPoint(abscissa.value(i+abscissaStart_), y.value(i+abscissaStart_), yType.value(i+abscissaStart_), 0.0);
		data->setZ(dataSets[n+displayDataSetStart_]->z());
	}
}

// Return index of first DisplayDataSet in this target
int DataSpaceRange::displayDataSetStart()
{
	return displayDataSetStart_;
}

// Return index of last DisplayDataSet in this target
int DataSpaceRange::displayDataSetEnd()
{
	return displayDataSetEnd_;
}

// Return number of sequential DisplayDataSets in this target
int DataSpaceRange::nDataSets()
{
	return nDataSets_;
}

// Return first abscissa index in this target
int DataSpaceRange::abscissaStart()
{
	return abscissaStart_;
}

// Return last abscissa index in this target
int DataSpaceRange::abscissaEnd()
{
	return abscissaEnd_;
}

// Return number of sequential abscissa values in this target
int DataSpaceRange::nPoints()
{
	return nPoints_;
}

// Return starting x value of range
double DataSpaceRange::xStart()
{
	// Check for valid collection
	if (values_.nItems() == 0)
	{
		msg.print("DataSpaceRange::xStart() - No values_ array.\n");
		return 0.0;
	}

	return values_.first()->x().first();
}

// Return final x value of range
double DataSpaceRange::xEnd()
{
	// Check for valid collection
	if (values_.nItems() == 0)
	{
		msg.print("DataSpaceRange::xEnd() - No values_ array.\n");
		return 0.0;
	}

	return values_.first()->x().last();
}

// Return starting z value of range
double DataSpaceRange::zStart()
{
	// Check for valid collection
	if (values_.nItems() == 0)
	{
		msg.print("DataSpaceRange::zStart() - No values_ array.\n");
		return 0.0;
	}

	return values_.first()->z();
}

// Return final z value of range
double DataSpaceRange::zEnd()
{
	// Check for valid collection
	if (values_.nItems() == 0)
	{
		msg.print("DataSpaceRange::zEnd() - No values_ array.\n");
		return 0.0;
	}

	return values_.last()->z();
}

/*
 * Values
 */

// Return reference y value specified
double DataSpaceRange::referenceY(int xIndex, int zIndex)
{
	DataSpaceData* data = values_[zIndex];
	if (data) return data->yReference().value(xIndex);
	else return 0.0;
}

// Return calculated y value specified
double DataSpaceRange::calculatedY(int xIndex, int zIndex)
{
	DataSpaceData* data = values_[zIndex];
	if (data) return data->yCalculated().value(xIndex);
	else return 0.0;
}

// Copy values from stored source collection, using index data provided
bool DataSpaceRange::copyValues(IndexData xIndex, IndexData zIndex)
{
	// Grab source abscissa values and display data array
	int nAbscissaPoints = parent_.sourceCollection()->displayAbscissa().nItems();
	List<DisplayDataSet>& dataSets = parent_.sourceCollection()->displayData();
	DisplayDataSet* dataSet;
	int actualZ, actualX;

	// Loop over z indices defined in range
	for (int z = 0; z < nDataSets_; ++z)
	{
		// Grab the dataSet that we want, taking into account the zIndex definition
		if (zIndex.type() == IndexData::NormalIndex) dataSet = dataSets[z+displayDataSetStart_];
		else if (zIndex.type() == IndexData::RelativeIndex)
		{
			// Check current index, accounting for offset defined in zIndex
			actualZ = displayDataSetStart_+z+zIndex.offset();
			if ((actualZ < 0) || (actualZ >= dataSets.nItems()))
			{
				msg.print("Warning: Relative offset defined for Z reference when copying values, and %i is out of range (available indices are 1 through %i).\n", actualZ+1, dataSets.nItems());
				dataSet = NULL;
			}
			else dataSet = dataSets[actualZ];
		}
		else if (zIndex.type() == IndexData::FixedIndex)
		{
			// Check current index, accounting for offset defined in zIndex
			actualZ = zIndex.index();
			if ((actualZ < 0) || (actualZ >= dataSets.nItems()))
			{
				msg.print("Warning: Absolute index defined for Z reference when copying values, and %i is out of range (available indices are 1 through %i).\n", actualZ+1, dataSets.nItems());
				dataSet = NULL;
			}
			else dataSet = dataSets[actualZ];
		}

		// Check validity of dataset - if none is set, zero the relevant values_ entry and move on
		if (!dataSet)
		{
			values_[z]->zeroYReference();
			continue;
		}

		// Grab data from dataSet
		const Array<double>& yRef = dataSet->y();

		// Loop over x indices defined in range
		if (xIndex.type() == IndexData::FixedIndex) actualX = xIndex.index();
		for (int x = 0; x<nPoints_; ++x)
		{
			// Calculate actual X value to use
			if (xIndex.type() == IndexData::NormalIndex) actualX = abscissaStart_+x;
			else if (xIndex.type() == IndexData::RelativeIndex) actualX = abscissaStart_+x+xIndex.offset();

			// Check index of x against valid abscissa range
			if ((x < 0) || (x >= nAbscissaPoints)) values_[z]->setReferenceY(x, 0.0);
			else values_[z]->setReferenceY(x, yRef.value(actualX));
		}
	}

	return true;
}

// Calculate values from specified equation
bool DataSpaceRange::calculateValues(Expression& equation, Variable* xVariable, Variable* zVariable, const RefList<ReferenceVariable,bool>& usedReferences)
{
	bool success;

	// Generate fitted data over all targetted datasets / abscissa values
	DataSpaceData* values = values_.first();

	// Grab abscissa data
	const Array<double>& x = values->x();

	// Loop over datasets
	for (int n=0; n<nDataSets_; ++n)
	{
		// Grab original data types
		const Array<DisplayDataSet::DataPointType>& yType = values->yType();

		// Set z variable value
		zVariable->set(values->z());
		
		// Loop over abscissa values
		for (int i=0; i<nPoints_; ++i)
		{
			// Nothing to do if this point does not exist...
			if (yType.value(i) == DisplayDataSet::NoPoint) continue;

			// Set x variable value	
			xVariable->set(x.value(i));

			// Generate reference values
			for (RefListItem<ReferenceVariable,bool>* ri = usedReferences.first(); ri != NULL; ri = ri->next) ri->item->updateValue(i, n);

			// Calculate and store y value
			values->setCalculatedY(i, equation.execute(success));
			if (!success) return false;
		}

		// Move to next calculated data element
		values = values->next;
	}
}

// Return sos error between stored and reference values
double DataSpaceRange::sosError()
{
	double sos = 0.0, yDiff;
	for (DataSpaceData* values = values_.first(); values != NULL; values = values->next)
	{
		// Grab arrays
		const Array<double>& yReference = values->yReference();
		const Array<double>& yCalculated = values->yCalculated();
		const Array<DisplayDataSet::DataPointType>& yType = values->yType();

		// Loop over abscissa values
		for (int i=0; i<nPoints_; ++i)
		{
			// Nothing to do if this point does not exist...
			if (yType.value(i) == DisplayDataSet::NoPoint) continue;
			
			yDiff = yReference.value(i) - yCalculated.value(i);

			sos += yDiff * yDiff;
		}
	}

	return sos;
}

// Copy calculated data to specified Collection
void DataSpaceRange::copyCalculatedValues(Collection* target)
{
	DataSet* newDataSet;
	for (DataSpaceData* values = values_.first(); values != NULL; values = values->next)
	{
		newDataSet = target->addDataSet();
		target->setDataSetData(newDataSet, values->x(), values->yCalculated());
		target->setDataSetZ(newDataSet, values->z());
	}
}
