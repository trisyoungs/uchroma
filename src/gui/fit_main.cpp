/*
	*** Fit - Main Function
	*** src/gui/fit_main.cpp
	Copyright T. Youngs 2012-2013

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
#include "gui/uchroma.h"
#include <parser/scopenode.h>

// Reset equation
void FitDialog::resetEquation()
{
	equation_.clear();
	xVariable_ = equation_.addGlobalVariable("x");
	yVariable_ = equation_.addGlobalVariable("y");
	zVariable_ = equation_.addGlobalVariable("z");
	equation_.setGenerateMissingVariables(true);
	equationValid_ = false;
}

// Update fit variables list
void FitDialog::updateFitVariables()
{
	// First, clear all 'used' flags
	EquationVariable* eqVar;
	for (eqVar = equationVariables_.first(); eqVar != NULL; eqVar = eqVar->next)
	{
		eqVar->setVariable(NULL);
		eqVar->setUsed(false);
	}
	nVariablesUsed_ = 0;
	fitVariables_.clear();

	// Now, loop over current variables in the equation_
	// Ignore 'x', 'y', and 'z' if they exist
	// If a variable already exists in equationVariables_, set it's 'used' status to true.
	// If it doesn't, create it and set it's 'used' status to true
	ScopeNode* rootNode = equation_.rootNode();
	for (Variable* var = rootNode->variables.variables(); var != NULL; var = var->next)
	{
		// Is this variable one of 'x', 'y', or 'z'?
		if (strcmp(var->name(),"x") == 0 || strcmp(var->name(),"y") == 0 || strcmp(var->name(),"z") == 0) continue;

		for (eqVar = equationVariables_.first(); eqVar != NULL; eqVar = eqVar->next) if (strcmp(eqVar->name(),var->name()) == 0) break;
		if (eqVar == NULL)
		{
			eqVar = equationVariables_.add();
			eqVar->setName(var->name());
		}

		// Update variable pointer
		eqVar->setVariable(var);
		eqVar->setUsed(true);
		fitVariables_.add(eqVar);
		++nVariablesUsed_;
	}
}

// Generate fitted data for current targets
bool FitDialog::generateFittedData()
{
	// Generate fitted data using current equation / variables
	// We will use the values of x / y stored in currentFittedData_, rather than the original fitData_.
	if (currentFittedData_ == NULL)
	{
		// Generate fitted data over all available fitData_
		ui.OutputEdit->append("Simultaneous fitting not yet enabled.");
		return false;
	}
	else
	{
		zVariable_->set(currentFittedData_->z());
		Array<double>& x = currentFittedData_->arrayX();
		Array<double>& y = currentFittedData_->arrayY();
		for (int n = 0; n<x.nItems(); ++n)
		{
			xVariable_->set(x[n]);
			yVariable_->set(0.0);
			y[n] = equation_.execute();
		}
	}
	return true;
}

// Generate SOS error for current targets
double FitDialog::sosError(Array<double>& alpha)
{
	// Poke current values back into the equation variables
	int n = 0;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next) ri->item->variable()->set(alpha[n++]);
	
	// Generate new data from current variable values
	generateFittedData();

	double yDiff, sos = 0.0;
	if (currentFittedData_ == NULL)
	{
		// Generate fitted data over all available fitData_
		ui.OutputEdit->append("Simultaneous fitting not yet enabled.");
		return 0.0;
	}
	else
	{
		Array<double>& yOrig = currentFitData_->arrayY();
		Array<double>& yNew = currentFittedData_->arrayY();
		for (n = 0; n<yOrig.nItems(); ++n)
		{
			yDiff = yOrig[n] - yNew[n];
			sos += yDiff * yDiff;
		}
	}
	return sos;
}

// Generate RMS error for current targets
double FitDialog::rmsError(Array<double>& alpha)
{
	// Poke current values back into the equation variables
	int n = 0;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next) ri->item->variable()->set(alpha[n++]);
	
	// Generate new data from current variable values
	generateFittedData();

	double yDiff, rms = 0.0;
	if (currentFittedData_ == NULL)
	{
		// Generate fitted data over all available fitData_
		ui.OutputEdit->append("Simultaneous fitting not yet enabled.");
		return 0.0;
	}
	else
	{
		Array<double>& yOrig = currentFitData_->arrayY();
		Array<double>& yNew = currentFittedData_->arrayY();
		for (n = 0; n<yOrig.nItems(); ++n)
		{
			yDiff = yOrig[n] - yNew[n];
			rms += yDiff * yDiff;
		}
		rms = sqrt(rms/yOrig.nItems());
	}
	return rms;
}

// Perform fitting with current settings
bool FitDialog::doFitting()
{
	// Grab source collection, and construct a list of data to fit, obeying all defined data limits
	fitData_.clear();
	fittedData_.clear();
	Collection* collection = uChroma_->collection(ui.SourceCollectionCombo->currentIndex());
	if (!collection) return false;
	if (ui.SourceXYSlicesRadio->isChecked())
	{
		// Source data is normal XY slices from the current collection
		double xMin = ui.SourceXYXMinSpin->value(), xMax = ui.SourceXYXMaxSpin->value();
		ui.OutputEdit->append("Setting up XY slice data over " + QString::number(xMin) + " < x < " + QString::number(xMax));

		for (int n=ui.SourceXYSliceFromSpin->value()-1; n<ui.SourceXYSliceToSpin->value(); ++n)
		{
			Data2D& oldData = collection->slice(n)->data();
			Data2D* newData = fitData_.add();
			newData->setZ(oldData.z());

			// Copy x-range specified
			for (int m=0; m<oldData.nPoints(); ++m)
			{
				if (oldData.x(m) < xMin) continue;
				if (oldData.x(m) > xMax) break;
				newData->addPoint(oldData.x(m), oldData.y(m));
			}

			// Copy the new data to fittedData_, our temporary fitting array
			Data2D* newFitData = fittedData_.add();
			(*newFitData) = (*newData);
		}
	}
	else if (ui.SourceYZSlicesRadio->isChecked())
	{
		// TODO
	}

	// Set up destination collection
	Collection* destinationCollection;
	if (ui.DestinationNewCollectionRadio->isChecked()) destinationCollection = uChroma_->addCollection("Fit Results");
	else
	{
		QVariant data = ui.DestinationCollectionCombo->itemData(ui.DestinationCollectionCombo->currentIndex());
		destinationCollection = uChroma_->collection(data.toInt());
		if (destinationCollection == NULL)
		{
			ui.OutputEdit->append("ERROR: Unable to set destination collection for fitting output.");
			return false;
		}
	}
	destinationCollection->clearSlices();

	// Construct alpha array (variables to fit) to pass to minimiser
        Array<double> alpha;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next)
	{
		// Grab variable pointer from FitVariable
		EquationVariable* var = ri->item;
		alpha.add(var->value());
	}

	// Fit all at once, or individual slices?
	if (ui.FitAtOnceCheck->isChecked())
	{
		currentFitData_ = NULL;

		// Call the relevant minimiser
		simplexMinimise(alpha);
	}
	else
	{
		currentFittedData_ = fittedData_.first();
		for (currentFitData_ = fitData_.first(); currentFitData_ != NULL; currentFitData_ = currentFitData_->next)
		{
			ui.OutputEdit->append("Fitting slice at z = " + QString::number(currentFitData_->z()));

			// Call the relevant minimiser
// 			simplexMinimise(alpha);
			sdMinimise(alpha);

			// Create new slice in destination collection
			Slice* newSlice = destinationCollection->addSlice();
			destinationCollection->setSliceZ(newSlice, currentFittedData_->z());
			Data2D& newData = newSlice->data();
			printf("Data of copying z = %f\n", currentFittedData_->z());
			newData = (*currentFittedData_);
			currentFittedData_ = currentFittedData_->next;
		}
	}

	// Update collection limits etc.
	destinationCollection->calculateDataLimits();
	destinationCollection->updateDataTransforms();

	return true;
}
