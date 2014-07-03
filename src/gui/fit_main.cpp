/*
	*** Fit - Main Function
	*** src/gui/fit_main.cpp
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
#include "gui/uchroma.h"
#include "parser/scopenode.h"
#include "templates/variantpointer.h"

// Reset equation
void FitWindow::resetEquation()
{
	equation_.clear();
	xVariable_ = equation_.addGlobalVariable("x");
	zVariable_ = equation_.addGlobalVariable("z");
	referenceYVariable_ = equation_.addGlobalVariable("refy");
	equation_.setGenerateMissingVariables(true);
	equationValid_ = false;
}

// Update fit variables list
void FitWindow::updateVariables()
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
		// Is this variable one of 'x', 'z', or 'refy'?
		if ((strcmp(var->name(),"x") == 0) || (strcmp(var->name(),"z") == 0) || (strcmp(var->name(),"refy") == 0)) continue;

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

// Update destination with current fitted data
void FitWindow::updateFittedData()
{
	// Check destination collection
	if (destinationCollection_ == NULL)
	{
		msg.print("Internal Error: No destination collection set in FitWindow::updateFittedData().\n");
		return;
	}

	// Clear existing slices
	destinationCollection_->clearDataSets();

	// Copy all slice data over
	for (FitTarget* fitTarget = fitTargets_.first(); fitTarget != NULL; fitTarget = fitTarget->next) fitTarget->copyCalculatedY(destinationCollection_);
}

// Generate SOS error for current targets
double FitWindow::sosError(Array<double>& alpha)
{
	// Poke current values back into the equation variables
	int n = 0;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next) ri->item->variable()->set(alpha[n++]);
	
	// Generate new data from current variable values
	currentFitTarget_->calculateY(equation_, xVariable_, zVariable_, referenceYVariable_, ui.SourceReferenceYCombo->currentIndex());

	// Get sos error from current fit target
	double sos = currentFitTarget_->sosError();

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
double FitWindow::rmsError(Array<double>& alpha)
{
	// Get sos error to start with
	double rms = sosError(alpha);

	// Normalise  to number of data points, and take sqrt
	int nPoints = currentFitTarget_->nDataSets() * currentFitTarget_->nPoints();

	return sqrt(rms/nPoints);
}

// Perform fitting with current settings
bool FitWindow::doFitting()
{
	// Check number of variables to fit
	if (fitVariables_.nItems() == 0)
	{
		printMessage("ERROR: No variables to fit!");
		return false;
	}

	// Check source collection
	if (!sourceCollection_)
	{
		printMessage("Internal Error: No sourceCollection_ pointer set.\n");
		return false;
	}
	
	// Construct a list of data to fit, obeying all defined data limits
	fitTargets_.clear();
	currentFitTarget_ = NULL;

	// Determine X bin range
	double xMin = ui.SourceXMinSpin->value(), xMax = ui.SourceXMaxSpin->value();
	const Array<double>& abscissa = sourceCollection_->displayAbscissa();
	int firstPoint, lastPoint;
	for (firstPoint = 0; firstPoint < abscissa.nItems(); ++firstPoint) if (abscissa.value(firstPoint) >= xMin) break;
	for (lastPoint = abscissa.nItems()-1; lastPoint >= 0; --lastPoint) if (abscissa.value(lastPoint) <= xMax) break;

	// Grab slice range
	int firstSlice, lastSlice;
	firstSlice = ui.SourceDataSetFromSpin->value() - 1;
	lastSlice = ui.SourceDataSetToSpin->value() - 1;

	// Construct source data
	if (ui.SourceXYSlicesRadio->isChecked())
	{
		// Source data is normal XY slices from the current collection
		printMessage("Setting up XY slice data over %e < x < %e", xMin, xMax);

		if (ui.OptionsGlobalFitCheck->isChecked())
		{
			FitTarget* target = fitTargets_.add();
			target->set(sourceCollection_, firstSlice, lastSlice, firstPoint, lastPoint);
		}
		else for (int n=firstSlice; n<=lastSlice; ++n) fitTargets_.add()->set(sourceCollection_, n, n, firstPoint, lastPoint);
	}
	else if (ui.SourceZYSlicesRadio->isChecked())
	{
		// Source data will be slices in the ZY plane
		printMessage("Setting up ZY slice data over %e < x < %e", xMin, xMax);

		if (ui.OptionsGlobalFitCheck->isChecked())
		{
			FitTarget* target = fitTargets_.add();
			target->set(sourceCollection_, firstSlice, lastSlice, firstPoint, lastPoint);
		}
		else for (int n=firstPoint; n<=lastPoint; ++n) fitTargets_.add()->set(sourceCollection_, firstSlice, lastSlice, n, n);
	}

	// Set up destination collection
	if (ui.DestinationNewCollectionRadio->isChecked())
	{
		destinationCollection_ = sourceCollection_->addFitData(ui.EquationEdit->text());
	}
	else
	{
		// Get selected destination collection
		Collection* destinationCollection_ = VariantPointer<Collection>(ui.DestinationCollectionCombo->itemData(ui.DestinationCollectionCombo->currentIndex()));
		if (destinationCollection_ == NULL)
		{
			printMessage("ERROR: Unable to set destination collection for fitting output.");
			return false;
		}
	}
	destinationCollection_->clearDataSets();

	// Loop over defined FitTargets (global fit has already been accounted for)
	bool result;
	for (currentFitTarget_ = fitTargets_.first(); currentFitTarget_ != NULL; currentFitTarget_ = currentFitTarget_->next)
	{
		printMessage("Fitting %i dataset(s) at %e < z < %e", currentFitTarget_->nDataSets(), currentFitTarget_->zStart(), currentFitTarget_->zEnd());

		// Call the minimiser
		result = minimise();

		// Copy final fitted data over to destination collection
		updateFittedData();
	}

	return result;
}

// Set source collection
void FitWindow::setSourceCollection(Collection* collection)
{
	if (sourceCollection_ == collection) return;

	sourceCollection_ = collection;
	
	updateSourceData(true);
}

/*
 * Minimisation
 */

// Minimise, calling relevant method
bool FitWindow::minimise()
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
