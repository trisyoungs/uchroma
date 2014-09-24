/*
	*** Fit Widget
	*** src/kernels/fit.h
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

#ifndef UCHROMA_FIT_H
#define UCHROMA_FIT_H

#include "base/equationvariable.h"
#include "base/referencevariable.h"
#include "expression/expression.h"
#include "base/dataspace.h"
#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
class UChromaWindow;
class Collection;
class DataSet;
class Variable;

/*
 * Fit Kernel
 */
class FitKernel
{
	public:
	// Constructor
	FitKernel();
	// Destructor
	~FitKernel();
	// Copy Constructor
	FitKernel(const FitKernel& source);
	// Assignment operator
	void operator=(const FitKernel& source);


	/*
	 * Equation and Variable Data
	 */
	private:
	// Local list of variables use in equations, including limits
	List<EquationVariable> variables_;
	// Number of variables which are used in the current equation
	int nVariablesUsed_;
	// Original text passed for last equation
	QString equationText_;
	// Fit equation
	Expression equation_;
	// Whether current equation is valid
	bool equationValid_;
	// Standard x and z variables
	Variable* xVariable_, *zVariable_;
	// List of variables targetted in fit process
	RefList<EquationVariable,bool> usedVariables_;
	// List of data references available
	List<ReferenceVariable> references_;
	// List of data reference used in fit
	RefList<ReferenceVariable,bool> usedReferences_;

	private:
	// Update variables list
	void updateVariables();

	public:
	// Reset equation
	void resetEquation();
	// Set equation, returning if it was parsed correctly
	bool setEquation(QString equation);
	// Return equation text
	QString equationText();
	// Return whether equation is valid
	bool equationValid();
	// Set equation as invalid
	void setEquationInvalid();
	// Return number of variables used in equation
	int nVariablesUsed();
	// Return first variable in list
	EquationVariable* variables();
	// Return first variable used in equation
	RefListItem<EquationVariable,bool>* usedVariables();
	// Return named variable, if it exists
	EquationVariable* variable(QString name);
	// Add reference variable to our list, and the relevant Tree scope
	ReferenceVariable* addReference(QString name);
	// Remove reference variable from the list, and the relevant Tree scope
	void removeReference(ReferenceVariable* refVar);
	// Return unique reference name based on the supplied string
	QString uniqueReferenceName(QString baseName);
	// Return first reference variable in list
	ReferenceVariable* references();
	// Return named reference, if it exists
	ReferenceVariable* reference(QString name);
	// Return first data reference used in fit
	RefListItem<ReferenceVariable,bool>* usedReferences();


	/*
	 * Fit Range
	 */
	public:
	// Range Type enum
	enum RangeType { AbsoluteRange, SinglePointRange, IndexRange, nRangeTypes };
	// Convert text string to RangeType
	static FitKernel::RangeType rangeType(QString s);
	// Convert RangeType to text string
	static const char* rangeType(FitKernel::RangeType id);

	private:
	// Type of x source range
	FitKernel::RangeType xRange_;
	// Minimum and maximum absolute x value to use for fitting
	double absoluteXMin_, absoluteXMax_;
	// Minimum, maximum, and single x point index to use for fitting
	int indexXMin_, indexXMax_, indexXSingle_;
	// Type of z source range
	FitKernel::RangeType zRange_;
	// Minimum and maximum absolute z value to use for fitting
	double absoluteZMin_, absoluteZMax_;
	// Minimum, maximum, and singlez dataset index to use for fitting
	int indexZMin_, indexZMax_, indexZSingle_;
	// Whether fit is to be performed across ZY rather than XY
	bool orthogonal_;
	// Whether all available datasets are to be fit simultaneously
	bool global_;
	// Data space to use in fit
	DataSpace fitSpace_;
	// Current data range for fitting
	DataSpaceRange* currentFitRange_;
	// Source collection for fitting
	Collection* sourceCollection_;
	// Destination collection for fitted data
	Collection* destinationCollection_;

	public:
	// Set type of x source range
	void setXRange(FitKernel::RangeType range);
	// Return type of x source range
	FitKernel::RangeType xRange();
	// Set minimum absolute x value to use for fitting
	void setAbsoluteXMin(double value);
	// Return minimum absolute x value to use for fitting
	double absoluteXMin();
	// Set maximum absolute x value to use for fitting
	void setAbsoluteXMax(double value);
	// Return maximum absolute x value to use for fitting
	double absoluteXMax();
	// Set minimum x point index to use for fitting
	void setIndexXMin(int index);
	// Return minimum x point index to use for fitting
	int indexXMin();
	// Set maximum x point index to use for fitting
	void setIndexXMax(int index);
	// Return maximum x point index to use for fitting
	int indexXMax();
	// Set single x point index to use for fitting
	void setIndexXSingle(int index);
	// Return single x point index to use for fitting
	int indexXSingle();
	// Set type of z source range
	void setZRange(FitKernel::RangeType range);
	// Return type of z source range
	FitKernel::RangeType zRange();
	// Set minimum absolute z value to use for fitting
	void setAbsoluteZMin(double value);
	// Return minimum absolute z value to use for fitting
	double absoluteZMin();
	// Set maximum absolute z value to use for fitting
	void setAbsoluteZMax(double value);
	// Return maximum absolute z value to use for fitting
	double absoluteZMax();
	// Set minimum z dataset index to use for fitting
	void setIndexZMin(int index);
	// Return minimum z dataset index to use for fitting
	int indexZMin();
	// Set maximum z dataset index to use for fitting
	void setIndexZMax(int index);
	// Return maximum z dataset index to use for fitting
	int indexZMax();
	// Set single z dataset index to use for fitting
	void setIndexZSingle(int index);
	// Return single z dataset index to use for fitting
	int indexZSingle();
	// Set whether fit is to be performed across ZY rather than XY
	void setOrthogonal(bool orthogonal);
	// Return whether fit is to be performed across ZY rather than XY
	bool orthogonal();
	// Set whether all available datasets are to be fit simultaneously
	void setGlobal(bool global);
	// Return whether all available datasets are to be fit simultaneously
	bool global();
	// Set source collection
	void setSourceCollection(Collection* collection);
	// Return source collection
	Collection* sourceCollection();
	// Set destination collection for fitted data
	void setDestinationCollection(Collection* collection);
	// Return destination collection for fitted data
	Collection* destinationCollection();
	// Update ranges to ensure sensible limits for current collection
	void checkRanges();


	/*
	 * Minimisation Setup
	 */
	public:
	// Minimisation methods
	enum MinimisationMethod { SteepestDescentMethod, SimplexMethod, nMinimisationMethods };
	// Convert text string to MinimisationMethod
	static FitKernel::MinimisationMethod minimisationMethod(const char* s);
	// Convert MinimisationMethod to text string
	static const char* minimisationMethod(FitKernel::MinimisationMethod id);

	private:
	// Minimisation method to use
	MinimisationMethod method_;
	// Convergence tolerance
	double tolerance_;
	// Maximum steps to use in minimisation
	int maxSteps_;
	// Strength of variable limits
	double limitStrength_;


	private:
	// Calculate SOS error for current targets
	double sosError(Array<double>& alpha);
	// Calculate RMS error for current targets
	double rmsError(Array<double>& alpha);
	// Simplex minimise
	bool simplexMinimise(Array<double>& alpha);
	// Steepest Descent minimise
	bool sdMinimise(Array<double>& alpha, double tolerance, int maxSteps);
	// Minimise, calling relevant method
	bool minimise();

	public:
	// Set minimisation method to use
	void setMethod(FitKernel::MinimisationMethod method);
	// Return minimisation method to use
	FitKernel::MinimisationMethod method();
	// Set convergence tolerance
	void setTolerance(double tolerance);
	// Return convergence tolerance
	double tolerance();
	// Set maximum steps to use in minimisation
	void setMaxSteps(int nSteps);
	// Return maximum steps to use in minimisation
	int maxSteps();
	// Set strength of variable limits
	void setLimitStrength(double strength);
	// Return strength of variable limits
	double limitStrength();
	// Perform fitting with current settings
	bool fit();
};

#endif
