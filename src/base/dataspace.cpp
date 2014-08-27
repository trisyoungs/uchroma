/*
	*** DataSpace
	*** src/base/dataspace.cpp
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

// Constructor
DataSpace::DataSpace()
{
	// Private variables
	sourceCollection_ = NULL;
	displayDataSetStart_ = -1;
	displayDataSetEnd_ = -1;
	nDataSets_ = 0;
	abscissaStart_ = -1;
	abscissaEnd_ = -1;
	nPoints_ = 0;
}

// Destructor
DataSpace::~DataSpace()
{
}

// Initialise ranges
bool DataSpace::initialise(Collection* sourceCollection, int xIndexMin, int xIndexMax, int zIndexMin, int zIndexMax, bool orthogonal, bool global)
{
	sourceCollection_ = sourceCollection;

	const Array<double>& abscissa = sourceCollection_->displayAbscissa();

	// Store indices and check limits
	displayDataSetStart_ = zIndexMin;
	displayDataSetEnd_ = zIndexMax;
	nDataSets_ = (displayDataSetEnd_ - displayDataSetStart_) + 1;
	if ((displayDataSetStart_ < 0) || (displayDataSetEnd_ >= sourceCollection_->nDataSets()) || (nDataSets_ < 1))
	{
		msg.print("Error: Invalid dataset range used to generate DataSpace data : %i < n < %i (available range= 0 to %i).\n", displayDataSetStart_, displayDataSetEnd_, sourceCollection_->nDataSets()-1); 
		return false;
	}
	abscissaStart_ = xIndexMin;
	abscissaEnd_ = xIndexMax;
	nPoints_ = (abscissaEnd_ - abscissaStart_) + 1;
	if ((abscissaStart_ < 0) || (abscissaEnd_ >= abscissa.nItems()) || (nPoints_ < 1))
	{
		msg.print("Error: Invalid abscissa range used to generate DataSpace data : %i < n < %i (available range= 0 to %i).\n", abscissaStart_, abscissaEnd_, abscissa.nItems()-1); 
		return false;
	}

	if (orthogonal)
	{
		msg.print("Setting up orthogonal (ZY) data over %e < x < %e and %e < z < %e\n", abscissa.value(abscissaStart_), abscissa.value(abscissaEnd_), sourceCollection_->dataSet(displayDataSetStart_)->data().z(), sourceCollection_->dataSet(displayDataSetEnd_)->data().z());

		if (global)
		{
			DataSpaceRange* range = ranges_.add(*this);
			range->set(sourceCollection_, abscissaStart_, abscissaEnd_, displayDataSetStart_, displayDataSetEnd_);
		}
		else for (int n = abscissaStart_; n<= abscissaEnd_; ++n) ranges_.add(*this)->set(sourceCollection_, n, n, displayDataSetStart_, displayDataSetEnd_);
	}
	else
	{
		// Source data is normal XY slices from the current collection
		msg.print("Setting up normal (XY) data over %e < x < %e and %e < z < %e\n", abscissa.value(abscissaStart_), abscissa.value(abscissaEnd_), sourceCollection_->dataSet(displayDataSetStart_)->data().z(), sourceCollection_->dataSet(displayDataSetEnd_)->data().z());

		if (global)
		{
			DataSpaceRange* range = ranges_.add(*this);
			range->set(sourceCollection_, abscissaStart_, abscissaEnd_, displayDataSetStart_, displayDataSetEnd_);
		}
		else for (int n = displayDataSetStart_; n<= displayDataSetEnd_; ++n) ranges_.add(*this)->set(sourceCollection_, abscissaStart_, abscissaEnd_, n, n);
	}
}

// Return source collection
Collection* DataSpace::sourceCollection()
{
	return sourceCollection_;
}

// Return range list
DataSpaceRange* DataSpace::ranges()
{
	return ranges_.first();
}

// Copy calculated y data to destination collection specified
void DataSpace::copy(Collection* destinationCollection)
{
	destinationCollection->clearDataSets();
	for (DataSpaceRange* fitRange = ranges_.first(); fitRange != NULL; fitRange = fitRange->next) fitRange->copyCalculatedValues(destinationCollection);
}