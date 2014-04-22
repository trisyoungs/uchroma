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
	zVariable_ = equation_.addGlobalVariable("z");
	equation_.setGenerateMissingVariables(true);
	equationValid_ = false;
}

// Update fit variables list
void FitDialog::updateVariables()
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
	// Ignore 'x' and 'z' if they exist
	// If a variable already exists in equationVariables_, set it's 'used' status to true.
	// If it doesn't, create it and set it's 'used' status to true
	ScopeNode* rootNode = equation_.rootNode();
	for (Variable* var = rootNode->variables.variables(); var != NULL; var = var->next)
	{
		// Is this variable one of 'x' or 'z'?
		if ((strcmp(var->name(),"x") == 0) || (strcmp(var->name(),"z") == 0)) continue;

		for (eqVar = equationVariables_.first(); eqVar != NULL; eqVar = eqVar->next) if (eqVar->name() == var->name()) break;
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
		for (Data2D* fittedData = fittedData_.first(); fittedData != NULL; fittedData = fittedData->next)
		{
			zVariable_->set(fittedData->z());
			Array<double>& x = fittedData->arrayX();
			Array<double>& y = fittedData->arrayY();
			for (int n = 0; n<x.nItems(); ++n)
			{
				xVariable_->set(x[n]);
				y[n] = equation_.execute();
			}
		}
	}
	else
	{
		zVariable_->set(currentFittedData_->z());
		Array<double>& x = currentFittedData_->arrayX();
		Array<double>& y = currentFittedData_->arrayY();
		for (int n = 0; n<x.nItems(); ++n)
		{
			xVariable_->set(x[n]);
			y[n] = equation_.execute();
		}
	}
	return true;
}

// Update destination with current fitted data
void FitDialog::updateFittedData()
{
	// Check destination collection
	if (destinationCollection_ == NULL)
	{
		msg.print("Internal Error: No destination collection set in FitDialog::updateFittedData().\n");
		return;
	}

	if (currentFittedData_ == NULL)
	{
		// Clear existing slices
		destinationCollection_->clearSlices();

		// Copy all slice data over
		for (Data2D* fittedData = fittedData_.first(); fittedData != NULL; fittedData = fittedData->next)
		{
			Slice* newSlice = destinationCollection_->addSlice();
			destinationCollection_->setSliceZ(newSlice, fittedData->z());
			destinationCollection_->setSliceData(newSlice, fittedData);
		}
	}
	else
	{
		// Update single slice
		if (destinationSlice_ == NULL)
		{
			msg.print("Internal Error: No destination slice set in FitDialog::updateFittedData().\n");
			return;
		}

		// Overwrite existing slice data
		destinationCollection_->setSliceData(destinationSlice_, currentFittedData_);
	}
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
		// Sum error over all fit data
		Data2D* fittedData = fittedData_.first();
		for (Data2D* fitData = fitData_.first(); fitData != NULL; fitData = fitData->next)
		{
			Array<double>& yOrig = fitData->arrayY();
			Array<double>& yNew = fittedData->arrayY();
			for (n = 0; n<yOrig.nItems(); ++n)
			{
				yDiff = yOrig[n] - yNew[n];
				sos += yDiff * yDiff;
			}

			fittedData = fittedData->next;
		}
	}
	else
	{
		// Sum error over current fit data
		Array<double>& yOrig = currentFitData_->arrayY();
		Array<double>& yNew = currentFittedData_->arrayY();
		for (n = 0; n<yOrig.nItems(); ++n)
		{
			yDiff = yOrig[n] - yNew[n];
			sos += yDiff * yDiff;
		}
	}

	// Calculate penalty from variables outside of their allowable ranges
	double penalty = 1.0;
	n = 0;
	EquationVariable* fitVar;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next, ++n)
	{
		// Grab variable from reflist item
		fitVar = ri->item;

		if (fitVar->minimumLimitEnabled() && (alpha[n] < fitVar->minimumLimit())) penalty += pow(fitVar->minimumLimit() - alpha[n], 2.0) * 1000.0;
		if (fitVar->maximumLimitEnabled() && (alpha[n] > fitVar->maximumLimit())) penalty += pow(alpha[n] - fitVar->maximumLimit(), 2.0) * 1000.0;
	}

	return sos * penalty;
}

// Generate RMS error for current targets
double FitDialog::rmsError(Array<double>& alpha)
{
	// Get sos error to start with
	double rms = sosError(alpha);

	// Normalise  to number of data points, and take sqrt
	int nPoints = 0;
	if (currentFittedData_ == NULL)
	{
		for (Data2D* data = fittedData_.first(); data != NULL; data = data->next) nPoints += data->nPoints();
	}
	else nPoints = currentFittedData_->nPoints();

	return sqrt(rms/nPoints);
}

// Perform fitting with current settings
bool FitDialog::doFitting()
{
	// Check number of variables to fit
	if (fitVariables_.nItems() == 0)
	{
		printMessage("ERROR: No variables to fit!");
		return false;
	}

	// Grab source collection, and construct a list of data to fit, obeying all defined data limits
	fitData_.clear();
	fittedData_.clear();
	Collection* collection = uChroma_->collection(ui.SourceCollectionCombo->currentIndex());
	if (!collection) return false;
	if (ui.SourceXYSlicesRadio->isChecked())
	{
		// Source data is normal XY slices from the current collection
		double xMin = ui.SourceXYXMinSpin->value(), xMax = ui.SourceXYXMaxSpin->value();
		printMessage("Setting up XY slice data over %e < x < %e", xMin, xMax);

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
	else if (ui.SourceZYSlicesRadio->isChecked())
	{
		// TODO
	}

	// Set up destination collection
	if (ui.DestinationNewCollectionRadio->isChecked()) destinationCollection_ = uChroma_->addCollection("Fit Results");
	else
	{
		QVariant data = ui.DestinationCollectionCombo->itemData(ui.DestinationCollectionCombo->currentIndex());
		destinationCollection_ = uChroma_->collection(data.toInt());
		if (destinationCollection_ == NULL)
		{
			printMessage("ERROR: Unable to set destination collection for fitting output.");
			return false;
		}
	}
	destinationCollection_->clearSlices();
	destinationSlice_ = NULL;

	// Fit all at once, or individual slices?
	bool result;
	if (ui.OptionsGlobalFitCheck->isChecked())
	{
		printMessage("Performing global fit over all source slices");

		currentFitData_ = NULL;
		currentFittedData_ = NULL;

		// Call the minimiser
		result = minimise();

		// Copy final fitted data over to destination collection
		updateFittedData();
	}
	else
	{
		currentFittedData_ = fittedData_.first();
		for (currentFitData_ = fitData_.first(); currentFitData_ != NULL; currentFitData_ = currentFitData_->next, currentFittedData_ = currentFittedData_->next)
		{
			printMessage("Fitting slice at z = %e", currentFitData_->z());

			// Create new slice target in destination collection
			destinationSlice_ = destinationCollection_->addSlice();
			destinationCollection_->setSliceZ(destinationSlice_, currentFittedData_->z());

			// Call the minimiser
			result = minimise();

			// Copy final fitted data over to destination collection
			updateFittedData();
		}
	}

	// Update collection limits etc.
	destinationCollection_->calculateDataLimits();
	destinationCollection_->updateDataTransforms();

	return result;
}

/*
 * Minimisation
 */

// Minimise, calling relevant method
bool FitDialog::minimise()
{
	// Construct alpha array (variables to fit) to pass to minimiser
	Array<double> alpha;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next)
	{
		// Grab variable pointer from FitVariable
		EquationVariable* var = ri->item;
		alpha.add(var->value());
	}

	// Call the minimiser
	bool result;
	switch (ui.MethodCombo->currentIndex())
	{
		// Steepest Descent
		case (0):
			result = sdMinimise(alpha, ui.MethodSDToleranceSpin->value(), ui.MethodSDMaxStepsSpin->value());
			break;
		// Simplex
		case (1):
			result = simplexMinimise(alpha);
			break;
	}

	// Print results...
	printMessage("Final, fitted parameters are:");
	int n = 0;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next, ++n)
	{
		// Grab variable pointer from FitVariable
		EquationVariable* var = ri->item;

		printMessage("\t%s\t=\t%e", qPrintable(var->name()), alpha[n]);
	}
	
	return result;
}
