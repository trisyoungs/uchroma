/*
	*** FitKernel
	*** src/kernels/fit.cpp
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

#include "kernels/fit.h"
#include "base/collection.h"
#include "base/currentproject.h"
#include "parser/scopenode.h"
#include "templates/variantpointer.h"

// Constructor
FitKernel::FitKernel()
{
	// Equation and Variable Data
	nVariablesUsed_ = 0;
	equationValid_ = false;
	xVariable_ = NULL;
	zVariable_ = NULL;
	referenceYVariable_ = NULL;

	xRange_ = FitKernel::AbsoluteRange;
	absoluteXMin_ = 0.0;
	absoluteXMax_ = 0.0;
	indexXSingle_ = 0;
	indexXMin_ = 0;
	indexXMax_ = 0;
	zRange_ = FitKernel::AbsoluteRange;
	absoluteZMin_ = 0.0;
	absoluteZMax_ = 0.0;
	indexZSingle_ = 0;
	indexZMin_ = 0;
	indexZMax_ = 0;
	orthogonal_ = false;
	global_ = false;
	currentFitTarget_ = NULL;
	sourceCollection_ = NULL;
	destinationCollection_ = NULL;

	// Minimisation Setup
	method_ = FitKernel::SteepestDescentMethod;
	limitStrength_ = 1.0e3;
	tolerance_ = 1.0e-3;
}

// Destructor
FitKernel::~FitKernel()
{
}

/*
 * Equation and Variable Data
 */

// Update fit variables list
void FitKernel::updateVariables()
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

// Reset equation
void FitKernel::resetEquation()
{
	equation_.clear();
	equationText_ = QString();
	xVariable_ = equation_.addGlobalVariable("x");
	zVariable_ = equation_.addGlobalVariable("z");
	referenceYVariable_ = equation_.addGlobalVariable("refy");
	equation_.setGenerateMissingVariables(true);
	equationValid_ = false;
}

// Set equation, returning if it was parsed correctly
bool FitKernel::setEquation(QString equation)
{
	equationText_ = equation;
	equationValid_ = equation_.setCommands(equationText_);

	updateVariables();

	CurrentProject::setAsModified();

	return equationValid_;
}

// Return equation text
QString FitKernel::equationText()
{
	return equationText_;
}

// Return whether equation is valid
bool FitKernel::equationValid()
{
	return equationValid_;
}

// Return number of variables used in equation
int FitKernel::nVariablesUsed()
{
	return nVariablesUsed_;
}

// Return first variable in list
EquationVariable* FitKernel::equationVariables()
{
	return equationVariables_.first();
}

// Return first variable used in equation
RefListItem<EquationVariable,bool>* FitKernel::fitVariables()
{
	return fitVariables_.first();
}

// Return named variable, if it exists
EquationVariable* FitKernel::variable(QString name)
{
	// Search list of variables for name provided
	for (EquationVariable* eqVar = equationVariables_.first(); eqVar != NULL; eqVar = eqVar->next) if (name == eqVar->name()) return eqVar;
	return NULL;
}

/*
 * Fit Targets
 */

// Range Types
const char* RangeTypeKeywords[] = { "Absolute", "SingleIndex", "IndexRange" };

// Convert text string to RangeType
FitKernel::RangeType FitKernel::rangeType(const char* s)
{
	for (int n=0; n<nRangeTypes; ++n) if (strcmp(RangeTypeKeywords[n],s)) return (FitKernel::RangeType) n;
	return FitKernel::nRangeTypes;
}

// Convert RangeType to text string
const char* FitKernel::rangeType(FitKernel::RangeType id)
{
	return  RangeTypeKeywords[id];
}

// Set type of x source range
void FitKernel::setXRange(FitKernel::RangeType range)
{
	xRange_ = range;

	CurrentProject::setAsModified();
}

// Return type of x source range
FitKernel::RangeType FitKernel::xRange()
{
	return xRange_;

	CurrentProject::setAsModified();
}

// Return minimum absolute x value to use for fitting
void FitKernel::setAbsoluteXMin(double value)
{
	absoluteXMin_ = value;
}

// Return minimum absolute x value to use for fitting
double FitKernel::absoluteXMin()
{
	return absoluteXMin_;
}

// Return maximum absolute x value to use for fitting
void FitKernel::setAbsoluteXMax(double value)
{
	absoluteXMax_ = value;

	CurrentProject::setAsModified();
}

// Return maximum absolute x value to use for fitting
double FitKernel::absoluteXMax()
{
	return absoluteXMax_;
}

// Return minimum x point index to use for fitting
void FitKernel::setIndexXMin(int index)
{
	indexXMin_ = index;

	CurrentProject::setAsModified();
}

// Return minimum x point index to use for fitting
int FitKernel::indexXMin()
{
	return indexXMin_;
}

// Return maximum x point index to use for fitting
void FitKernel::setIndexXMax(int index)
{
	indexXMax_ = index;

	CurrentProject::setAsModified();
}

// Return maximum x point index to use for fitting
int FitKernel::indexXMax()
{
	return indexXMax_;
}

// Set single x point index to use for fitting
void FitKernel::setIndexXSingle(int index)
{
	indexXSingle_ = index;

	CurrentProject::setAsModified();
}

// Return single x point index to use for fitting
int FitKernel::indexXSingle()
{
	return indexXSingle_;
}

// Set type of z source range
void FitKernel::setZRange(FitKernel::RangeType range)
{
	zRange_ = range;

	CurrentProject::setAsModified();
}

// Return type of z source range
FitKernel::RangeType FitKernel::zRange()
{
	return zRange_;
}

// Return minimum absolute z value to use for fitting
void FitKernel::setAbsoluteZMin(double value)
{
	absoluteZMin_ = value;

	CurrentProject::setAsModified();
}

// Return minimum absolute z value to use for fitting
double FitKernel::absoluteZMin()
{
	return absoluteZMin_;
}

// Return maximum absolute z value to use for fitting
void FitKernel::setAbsoluteZMax(double value)
{
	absoluteZMax_ = value;

	CurrentProject::setAsModified();
}

// Return maximum absolute z value to use for fitting
double FitKernel::absoluteZMax()
{
	return absoluteZMax_;
}

// Return minimum z dataset index to use for fitting
void FitKernel::setIndexZMin(int index)
{
	indexZMin_ = index;

	CurrentProject::setAsModified();
}

// Return minimum z dataset index to use for fitting
int FitKernel::indexZMin()
{
	return indexZMin_;
}

// Return maximum z dataset index to use for fitting
void FitKernel::setIndexZMax(int index)
{
	indexZMax_ = index;

	CurrentProject::setAsModified();
}

// Return maximum z dataset index to use for fitting
int FitKernel::indexZMax()
{
	return indexZMin();
}

// Set single z point index to use for fitting
void FitKernel::setIndexZSingle(int index)
{
	indexZSingle_ = index;

	CurrentProject::setAsModified();
}

// Return single z dataset index to use for fitting
int FitKernel::indexZSingle()
{
	return indexZSingle_;
}

// Return whether fit is to be performed across ZY rather than XY
void FitKernel::setOrthogonal(bool orthogonal)
{
	orthogonal_ = orthogonal;

	CurrentProject::setAsModified();
}

// Return whether fit is to be performed across ZY rather than XY
bool FitKernel::orthogonal()
{
	return orthogonal_;
}

// Return whether all available datasets are to be fit simultaneously
void FitKernel::setGlobal(bool global)
{
	global_ = global;

	CurrentProject::setAsModified();
}

// Return whether all available datasets are to be fit simultaneously
bool FitKernel::global()
{
	return global_;
}

// Set source collection
void FitKernel::setSourceCollection(Collection* collection)
{
	sourceCollection_ = collection;
	if (!sourceCollection_) return;
	
	// Set some defaults based on the source collection
	absoluteXMin_ = sourceCollection_->dataMin().x;
	absoluteXMax_ = sourceCollection_->dataMax().x;
	indexXMin_ = 0;
	indexXMax_ = sourceCollection_->displayAbscissa().nItems()-1;
	indexXSingle_ = 0;
	absoluteZMin_ = sourceCollection_->dataMin().z;
	absoluteZMax_ = sourceCollection_->dataMax().z;
	indexZMin_ = 0;
	indexZMax_ = sourceCollection_->nDataSets()-1;
	indexZSingle_ = 0;

	CurrentProject::setAsModified();
}

// Return source collection
Collection* FitKernel::sourceCollection()
{
	return sourceCollection_;
}

// Set destination collection for fitted data
void FitKernel::setDestinationCollection(Collection* collection)
{
	destinationCollection_ = collection;
}

// Return destination collection for fitted data
Collection* FitKernel::destinationCollection()
{
	return destinationCollection_;
}

// Update ranges to ensure sensible limits for current collection
void FitKernel::checkRanges()
{
	if (!sourceCollection_) return;

	// Check indices for x and z ranges
	if (indexXMin_ < 0) indexXMin_ = 0;
	else if (indexXMax_ >= sourceCollection_->displayAbscissa().nItems()) indexXMax_ = sourceCollection_->displayAbscissa().nItems()-1;
	if (indexZMin_ < 0) indexZMin_ = 0;
	else if (indexZMax_ >= sourceCollection_->nDataSets()) indexZMax_ = sourceCollection_->nDataSets()-1;
}

/*
 * Minimisation Setup
 */

// Minimisation methods
const char* MinimisationMethodKeywords[] = { "SteepestDescent", "Simplex" };

// Convert text string to MinimisationMethod
FitKernel::MinimisationMethod FitKernel::minimisationMethod(const char* s)
{
	for (int n=0; n<nMinimisationMethods; ++n) if (strcmp(MinimisationMethodKeywords[n],s)) return (FitKernel::MinimisationMethod) n;
	return FitKernel::nMinimisationMethods;
}

// Convert MinimisationMethod to text string
const char* FitKernel::minimisationMethod(FitKernel::MinimisationMethod id)
{
	return  MinimisationMethodKeywords[id];
}

// Update destination with current fitted data
void FitKernel::updateFittedData()
{
	// Check destination collection
	if (destinationCollection_ == NULL)
	{
		msg.print("Internal Error: No destination collection set in FitKernel::updateFittedData().\n");
		return;
	}

	// Clear existing slices
	destinationCollection_->clearDataSets();

	// Copy all slice data over
	for (FitTarget* fitTarget = fitTargets_.first(); fitTarget != NULL; fitTarget = fitTarget->next) fitTarget->copyCalculatedY(destinationCollection_);
}

// Generate SOS error for current targets
double FitKernel::sosError(Array<double>& alpha)
{
	// Poke current values back into the equation variables
	int n = 0;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next) ri->item->variable()->set(alpha[n++]);
	
	// Generate new data from current variable values
	currentFitTarget_->calculateY(equation_, xVariable_, zVariable_);

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
double FitKernel::rmsError(Array<double>& alpha)
{
	// Get sos error to start with
	double rms = sosError(alpha);

	// Normalise  to number of data points, and take sqrt
	int nPoints = currentFitTarget_->nDataSets() * currentFitTarget_->nPoints();

	return sqrt(rms/nPoints);
}

// Minimise, calling relevant method
bool FitKernel::minimise()
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
	switch (method_)
	{
		// Steepest Descent
		case (0):
			result = sdMinimise(alpha, tolerance_, maxSteps_);
			break;
		// Simplex
		case (1):
			result = simplexMinimise(alpha);
			break;
	}

	// Print results...
	msg.print("Final, fitted parameters are:\n");
	int n = 0;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next)
	{
		// Grab variable pointer from FitVariable
		EquationVariable* var = ri->item;

		msg.print("\t%s\t=\t%e", qPrintable(var->name()), alpha[n]);

		++n;
	}
	
	return result;
}

// Set minimisation method to use
void FitKernel::setMethod(FitKernel::MinimisationMethod method)
{
	method_ = method;
}

// Return minimisation method to use
FitKernel::MinimisationMethod FitKernel::method()
{
	return method_;
}

// Set convergence tolerance
void FitKernel::setTolerance(double tolerance)
{
	tolerance_ = tolerance;
}

// Return convergence tolerance
double FitKernel::tolerance()
{
	return tolerance_;
}

// Set maximum steps to use in minimisation
void FitKernel::setMaxSteps(int nSteps)
{
	maxSteps_ = nSteps;
}

// Return maximum steps to use in minimisation
int FitKernel::maxSteps()
{
	return maxSteps_;
}

// Set strength of variable limits
void FitKernel::setLimitStrength(double strength)
{
	limitStrength_ = strength;
}

// Return strength of variable limits
double FitKernel::limitStrength()
{
	return limitStrength_;
}

// Perform fitting with current settings
bool FitKernel::fit()
{
	// Check number of variables to fit
	if (fitVariables_.nItems() == 0)
	{
		msg.print("Error: No variables to fit!\n");
		return false;
	}

	// Check source collection
	if (!sourceCollection_)
	{
		msg.print("Internal Error: No sourceCollection_ pointer set.\n");
		return false;
	}
	
	// Construct a list of data to fit, obeying all defined data limits
	fitTargets_.clear();
	currentFitTarget_ = NULL;

	// Determine X bin index range
	int firstXPoint, lastXPoint;
	const Array<double>& abscissa = sourceCollection_->displayAbscissa();
	if (xRange_ == FitKernel::AbsoluteRange)
	{
		for (firstXPoint = 0; firstXPoint < (abscissa.nItems()-1); ++firstXPoint) if (abscissa.value(firstXPoint) >= absoluteXMin_) break;
		for (lastXPoint = abscissa.nItems()-1; lastXPoint > 0; --lastXPoint) if (abscissa.value(lastXPoint) <= absoluteXMax_) break;
	}
	else if (xRange_ == FitKernel::SinglePointRange)
	{
		firstXPoint = indexXSingle_ - 1;
		lastXPoint = firstXPoint;
	}
	else
	{
		firstXPoint = indexXMin_ - 1;
		lastXPoint = indexXMax_ - 1;
	}

	// Determine Z (dataset) bin index range
	int firstZPoint = 0, lastZPoint = 0;
	if (zRange_ == FitKernel::AbsoluteRange)
	{
		for (firstZPoint = 0; firstZPoint < (sourceCollection_->nDataSets()-1); ++firstZPoint) if (sourceCollection_->dataSet(firstZPoint)->data().z() >= absoluteZMin_) break;
		for (lastZPoint = sourceCollection_->nDataSets()-1; lastZPoint > 0; --lastZPoint) if (sourceCollection_->dataSet(lastZPoint)->data().z() <= absoluteZMax_) break;
	}
	else if (zRange_ == FitKernel::SinglePointRange)
	{
		firstZPoint = indexZSingle_ - 1;
		lastZPoint = firstZPoint;
	}
	else
	{
		firstZPoint = indexZMin_ - 1;
		lastZPoint = indexZMax_ - 1;
	}

	// Construct source data
	if (orthogonal_)
	{
		// Source data will be slices in the ZY plane
		msg.print("Setting up orthogonal (ZY) data over %e < x < %e and %e < z < %e\n", abscissa.value(firstXPoint), abscissa.value(lastXPoint), sourceCollection_->dataSet(firstZPoint)->data().z(), sourceCollection_->dataSet(lastZPoint)->data().z());

		if (global_)
		{
			FitTarget* target = fitTargets_.add();
			target->set(sourceCollection_, firstZPoint, lastZPoint, firstXPoint, lastXPoint);
		}
		else for (int n= firstXPoint; n<= lastXPoint; ++n) fitTargets_.add()->set(sourceCollection_, firstZPoint, lastZPoint, n, n);
	}
	else
	{
		// Source data is normal XY slices from the current collection
		msg.print("Setting up normal (XY) data over %e < x < %e and %e < z < %e\n", abscissa.value(firstXPoint), abscissa.value(lastXPoint), sourceCollection_->dataSet(firstZPoint)->data().z(), sourceCollection_->dataSet(lastZPoint)->data().z());

		if (global_)
		{
			FitTarget* target = fitTargets_.add();
			target->set(sourceCollection_, firstZPoint, lastZPoint, firstXPoint, lastXPoint);
		}
		else for (int n = firstZPoint; n<= lastZPoint; ++n) fitTargets_.add()->set(sourceCollection_, n, n, firstXPoint, lastXPoint);
	}

	// Set up destination collection
	if (destinationCollection_ == NULL)
	{
		msg.print("Error: Destination collection has not been set in FitKernel (source collection is '%s'.\n", qPrintable(sourceCollection_->title()));
		return false;
	}
	destinationCollection_->clearDataSets();

	// Loop over defined FitTargets (global fit has already been accounted for)
	bool result;
	for (currentFitTarget_ = fitTargets_.first(); currentFitTarget_ != NULL; currentFitTarget_ = currentFitTarget_->next)
	{
		msg.print("Fitting %i dataset(s) at %e < z < %e\n", currentFitTarget_->nDataSets(), currentFitTarget_->zStart(), currentFitTarget_->zEnd());

		// Call the minimiser
		result = minimise();

		// Copy final fitted data over to destination collection
		updateFittedData();
	}

	return result;
}