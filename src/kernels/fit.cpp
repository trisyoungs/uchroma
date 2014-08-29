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
#include "expression/variable.h"
#include "base/collection.h"
#include "base/currentproject.h"
#include "templates/variantpointer.h"

// Constructor
FitKernel::FitKernel()
{
	// Equation and Variable Data
	nVariablesUsed_ = 0;
	equationValid_ = false;
	xVariable_ = equation_.createVariable("x", NULL, true);
	zVariable_ = equation_.createVariable("z", NULL, true);
	equation_.setGenerateMissingVariables(true);

	// Fit Range
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
	currentFitRange_ = NULL;
	sourceCollection_ = NULL;
	destinationCollection_ = NULL;

	// Minimisation Setup
	method_ = FitKernel::SteepestDescentMethod;
	maxSteps_ = 100;
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
	ReferenceVariable* refVar;
	for (eqVar = variables_.first(); eqVar != NULL; eqVar = eqVar->next) eqVar->resetVariable();
	nVariablesUsed_ = 0;
	for (refVar = references_.first(); refVar != NULL; refVar = refVar->next) refVar->resetVariable();
	usedVariables_.clear();
	usedReferences_.clear();

	// Now, loop over current variables in the equation_
	// Ignore 'x' and 'z' if they exist
	// If a variable already exists in equationVariables_, set it's 'used' status to true.
	// If it doesn't, create it and set it's 'used' status to true
	for (RefListItem<Variable,bool>* ri = equation_.variables(); ri != NULL; ri = ri->next)
	{
		Variable* var = ri->item;

		// Is this variable one of 'x' or 'z'? Must compare by name since the pointers are not permanent...
		if ((strcmp(var->name(),"x") == 0) || (strcmp(var->name(),"z") == 0)) continue;

		// Is it one of the reference variables that we created?
		ReferenceVariable* refVar = reference(var->name());
		if (refVar != NULL)
		{
			msg.print("Found reference variable '%s' in our list...\n", var->name());
			usedReferences_.add(refVar);
			continue;
		}

		// Must be a normal variable - search the EquationVariable list to see if it's already in there... if not, add it.
		eqVar = variable(var->name());
		if (eqVar == NULL)
		{
			eqVar = variables_.add();
			eqVar->setName(var->name());
		}

		// Update variable pointer
		eqVar->setVariable(var);
		eqVar->setUsed(true);
		usedVariables_.add(eqVar);
		++nVariablesUsed_;
	}
}

// Reset equation
void FitKernel::resetEquation()
{
	equation_.clear();
	equationText_ = QString();
	equationValid_ = false;
}

// Set equation, returning if it was parsed correctly
bool FitKernel::setEquation(QString equation)
{
	equationText_ = equation;
	equationValid_ = equation_.generate(qPrintable(equationText_), true);

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

// Set equation as invalid
void FitKernel::setEquationInvalid()
{
	equationValid_ = false;
}

// Return number of variables used in equation
int FitKernel::nVariablesUsed()
{
	return nVariablesUsed_;
}

// Return first variable in list
EquationVariable* FitKernel::variables()
{
	return variables_.first();
}

// Return first variable used in equation
RefListItem<EquationVariable,bool>* FitKernel::usedVariables()
{
	return usedVariables_.first();
}

// Return named variable, if it exists
EquationVariable* FitKernel::variable(QString name)
{
	// Search list of variables for name provided
	for (EquationVariable* eqVar = variables_.first(); eqVar != NULL; eqVar = eqVar->next) if (name == eqVar->name()) return eqVar;
	return NULL;
}

// Add reference variable to our list
ReferenceVariable* FitKernel::addReference(QString name)
{
	// Check that a reference of this name doesn't already exist
	if (reference(name))
	{
		msg.print("Error: Tried to add a reference with the same name ('%s') as one that already exists.\n", qPrintable(name));
		return NULL;
	}

	ReferenceVariable* refVar = references_.add();
	refVar->setSourceCollection(sourceCollection_);
	refVar->setName(name);

	return refVar;
}

// Remove reference variable from the list, and our Tree's global scope
void FitKernel::removeReference(ReferenceVariable* refVar)
{
	if (!references_.contains(refVar))
	{
		msg.print("Internal Error: Tried to remove a ReferenceVariable (%p) from a list that didn't own it.\n", refVar);
		return;
	}

	// Remove any associated variable first
	if (refVar->used())
	{
		equationValid_ = false;
		equation_.clear();
	}

	// Remove the variable
	references_.remove(refVar);
}

// Return unique reference name based on the supplied string
QString FitKernel::uniqueReferenceName(QString baseName)
{
	QString testName = baseName, suffix;
	int index = 0;
	do
	{
		// Add on suffix (if index > 0)
		if (index > 0) testName = baseName + "_"+QString::number(index);
		++index;
	} while (reference(testName));

	return testName;
}

// Return first reference variable in list
ReferenceVariable* FitKernel::references()
{
	return references_.first();
}

// Return named reference, if it exists
ReferenceVariable* FitKernel::reference(QString name)
{
	for (ReferenceVariable* refVar = references_.first(); refVar != NULL; refVar = refVar->next) if (refVar->name() == name) return refVar;
	return NULL;
}

// Return first data reference used in fit
RefListItem<ReferenceVariable,bool>* FitKernel::usedReferences()
{
	return usedReferences_.first();
}

/*
 * Fit Targets
 */

// Range Types
const char* RangeTypeKeywords[] = { "Absolute", "SingleIndex", "IndexRange" };

// Convert text string to RangeType
FitKernel::RangeType FitKernel::rangeType(const char* s)
{
	for (int n=0; n<FitKernel::nRangeTypes; ++n) if (strcmp(RangeTypeKeywords[n],s) == 0) return (FitKernel::RangeType) n;
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
	printf("Set xrange to %i\n", range);
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
	for (int n=0; n<nMinimisationMethods; ++n) if (strcmp(MinimisationMethodKeywords[n],s) == 0) return (FitKernel::MinimisationMethod) n;
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
	fitSpace_.copy(destinationCollection_);
}

// Generate SOS error for current targets
double FitKernel::sosError(Array<double>& alpha)
{
	// Poke current values back into the equation variables
	int n = 0;
	for (RefListItem<EquationVariable,bool>* ri = usedVariables_.first(); ri != NULL; ri = ri->next) ri->item->variable()->set(alpha[n++]);
	
	// Generate new data from current variable values
	currentFitRange_->calculateValues(equation_, xVariable_, zVariable_);

	// Get sos error from current fit target
	double sos = currentFitRange_->sosError();

	// Calculate penalty from variables outside of their allowable ranges
	double penalty = 1.0;
	n = 0;
	EquationVariable* fitVar;
	for (RefListItem<EquationVariable,bool>* ri = usedVariables_.first(); ri != NULL; ri = ri->next, ++n)
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
	int nPoints = currentFitRange_->nDataSets() * currentFitRange_->nPoints();

	return sqrt(rms/nPoints);
}

// Minimise, calling relevant method
bool FitKernel::minimise()
{
	// Construct alpha array (variables to fit) to pass to minimiser
	Array<double> alpha;
	for (RefListItem<EquationVariable,bool>* ri = usedVariables_.first(); ri != NULL; ri = ri->next)
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
	for (RefListItem<EquationVariable,bool>* ri = usedVariables_.first(); ri != NULL; ri = ri->next)
	{
		// Grab variable pointer from FitVariable
		EquationVariable* var = ri->item;

		msg.print("\t%s\t=\t%e\n", qPrintable(var->name()), alpha[n]);

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
	if (usedVariables_.nItems() == 0)
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
		firstXPoint = indexXSingle_;
		lastXPoint = firstXPoint;
	}
	else
	{
		firstXPoint = indexXMin_;
		lastXPoint = indexXMax_;
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
		firstZPoint = indexZSingle_;
		lastZPoint = firstZPoint;
	}
	else
	{
		firstZPoint = indexZMin_;
		lastZPoint = indexZMax_;
	}

	// Construct source data
	if (!fitSpace_.initialise(sourceCollection_, firstXPoint, lastXPoint, firstZPoint, lastZPoint, orthogonal_, global_)) return false;

	// Set up destination collection
	if (destinationCollection_ == NULL)
	{
		msg.print("Error: Destination collection has not been set in FitKernel (source collection is '%s'.\n", qPrintable(sourceCollection_->title()));
		return false;
	}
	destinationCollection_->clearDataSets();

	// Loop over defined DataRanges (global fit has already been accounted for)
	bool result;
	for (currentFitRange_ = fitSpace_.ranges(); currentFitRange_ != NULL; currentFitRange_ = currentFitRange_->next)
	{
		msg.print("Fitting range (%e < x < %e) (%e < z < %e)\n", currentFitRange_->xStart(), currentFitRange_->xEnd(), currentFitRange_->zStart(), currentFitRange_->zEnd());

		// Call the minimiser
		result = minimise();

		// Copy final fitted data over to destination collection
		updateFittedData();
	}

	return result;
}