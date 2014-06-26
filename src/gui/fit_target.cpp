/*
	*** Fit Target Functions
	*** src/gui/fit_target.cpp
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

#include "gui/fit.h"
#include <base/collection.h>

// Constructor
FitTarget::FitTarget() : ListItem<FitTarget>()
{
	collection_ = NULL;
	displayDataSetStart_ = -1;
	displayDataSetEnd_ = -1;
	nDataSets_ = 0;
	abscissaStart_ = -1;
	abscissaEnd_ = -1;
	nPoints_ = 0;
}

// Destructor
FitTarget::~FitTarget()
{
}

/* 
 * Target Data
 */

// Set target information
void FitTarget::set(Collection* collection, int firstDataSet, int lastDataSet, int abscissaFirst, int abscissaLast)
{
	collection_ = collection;
	displayDataSetStart_ = firstDataSet;
	displayDataSetEnd_ = lastDataSet;
	nDataSets_ = (displayDataSetEnd_ - displayDataSetStart_) + 1;
	abscissaStart_ = abscissaFirst;
	abscissaEnd_ = abscissaLast;
	nPoints_ = (abscissaEnd_ - abscissaStart_) + 1;

	// Setup calculated data arrays
	calculatedY_.clear();
	const Array<double>& abscissa = collection_->displayAbscissa();
	DisplayDataSet** dataSets = collection_->displayData().array();
	for (int n=0; n<nDataSets_; ++n)
	{
		Data2D* data = calculatedY_.add();
		for (int i=0; i<nPoints_; ++i) data->addPoint(abscissa.value(i+abscissaStart_), 0.0);
		data->setZ(dataSets[n]->z());
	}
}

// Return index of first DisplayDataSet to be fit
int FitTarget::displayDataSetStart()
{
	return displayDataSetStart_;
}

// Return index of last DisplayDataSet to be fit
int FitTarget::displayDataSetEnd()
{
	return displayDataSetEnd_;
}

// Return number of sequential DisplayDataSets to use in fitting
int FitTarget::nDataSets()
{
	return nDataSets_;
}

// Return first abscissa index to use in fitting
int FitTarget::abscissaStart()
{
	return abscissaStart_;
}

// Return last abscissa index to use in fitting
int FitTarget::abscissaEnd()
{
	return abscissaEnd_;
}

// Return number of sequential abscissa values to use in fitting
int FitTarget::nPoints()
{
	return nPoints_;
}

// Return z value of first slice used in fitting
double FitTarget::zStart()
{
	// Check for valid collection
	if (calculatedY_.nItems() == 0)
	{
		msg.print("FitTarget::zStart() - No calculatedY_ array.\n");
		return 0.0;
	}

	return calculatedY_.first()->z();
}

// Return z value of last slice used in fitting
double FitTarget::zEnd()
{
	// Check for valid collection
	if (calculatedY_.nItems() == 0)
	{
		msg.print("FitTarget::zEnd() - No calculatedY_ array.\n");
		return 0.0;
	}

	return calculatedY_.last()->z();
}

// Calculate 'fitted' values from specified equation
bool FitTarget::calculateY(Tree& equation, Variable* xVariable, Variable* zVariable, Variable* referenceYVariable, int referenceYIndex)
{
	// Generate fitted data over all targetted datasets / abscissa values
	int sliceIndex;
	DisplayDataSet** dataSets = collection_->displayData().array();
	DisplayDataSet* dataSet;
	Data2D* calcY = calculatedY_.first();

	// Grab abscissa and reference Y data
	const Array<double>& referenceY = dataSets[referenceYIndex]->y();
	const Array<double>& x = collection_->displayAbscissa();

	// Loop over datasets
	for (int n=0; n<nDataSets_; ++n)
	{
		// Get slice index, and grab dataset pointer
		sliceIndex = displayDataSetStart_ + n;
		dataSet = dataSets[sliceIndex];
		if (dataSet == NULL)
		{
			msg.print("FitTarget::calculateY() - No valid dataset for n = %i (index = %i)\n", n, sliceIndex);
			return false;
		}

		// Grab original data
		const Array<DisplayDataSet::DataPointType>& yType = dataSet->yType();

		// Set z variable value
		zVariable->set(dataSet->z());
		
		// Loop over abscissa values
		for (int i=0; i<nPoints_; ++i)
		{
			// Nothing to do if this point does not exist...
			if (yType.value(i+abscissaStart_) == DisplayDataSet::NoPoint) continue;

			// Set reference y variable value
			referenceYVariable->set(referenceY.value(i+abscissaStart_));

			// Set x variable value	
			xVariable->set(x.value(i+abscissaStart_));

			// Calculate and store y value
			calcY->setY(i, equation.execute());
		}

		// Move to next calculated data element
		calcY = calcY->next;
	}
}

// Return sos error between calculated and original y values
double FitTarget::sosError()
{
	int sliceIndex;
	DisplayDataSet** dataSets = collection_->displayData().array();
	DisplayDataSet* dataSet;
	Data2D* calcY = calculatedY_.first();
	const Array<double>& x = collection_->displayAbscissa();
	double sos = 0.0, yDiff;
	for (int n=0; n<nDataSets_; ++n)
	{
		// Get dataset index, and grab dataset pointer
		sliceIndex = displayDataSetStart_ + n;
		dataSet = dataSets[sliceIndex];
		if (dataSet == NULL)
		{
			msg.print("FitTarget::sosError() - No valid slice for n = %i (index = %i)\n", n, sliceIndex);
			return 0.0;
		}

		// Grab original data
		const Array<double>& y = dataSet->y();
		const Array<DisplayDataSet::DataPointType>& yType = dataSet->yType();

		// Loop over abscissa values
		for (int i=0; i<nPoints_; ++i)
		{
			// Nothing to do if this point does not exist...
			if (yType.value(i+abscissaStart_) == DisplayDataSet::NoPoint) continue;
			
			yDiff = y.value(i+abscissaStart_) - calcY->y(i);

			sos += yDiff * yDiff;
		}

		// Move to next calculated data element
		calcY = calcY->next;
	}

	return sos;
}

// Copy calculated Y data to specified Collection
void FitTarget::copyCalculatedY(Collection* target)
{
	int sliceIndex;
	DisplayDataSet* dataSet;
	DataSet* newDataSet;
	const Array<double>& x = collection_->displayAbscissa();
	for (int n=0; n<nDataSets_; ++n)
	{
		// Get slice index, and grab slice pointer
		sliceIndex = displayDataSetStart_ + n;
		dataSet = collection_->displayData()[sliceIndex];
		if (dataSet == NULL)
		{
			msg.print("FitTarget::copyCalculatedY() - No valid slice for n = %i (index = %i)\n", n, sliceIndex);
			return;
		}

		// Grab z value
		newDataSet = target->addDataSet();
		target->setDataSetData(newDataSet, calculatedY_[n]);
		target->setDataSetZ(newDataSet, dataSet->z());
	}
}
