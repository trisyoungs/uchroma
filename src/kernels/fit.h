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

#include "base/dnchar.h"
#include "base/data2d.h"
#include "base/equationvariable.h"
#include "parser/tree.h"
#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
class UChromaWindow;
class Collection;
class DataSet;
class Variable;

/*
 * Fit Target
 */
class FitTarget : public ListItem<FitTarget>
{
	public:
	// Constructor / Destructor
	FitTarget();
	~FitTarget();


	/* 
	 * Target Data
	 */
	private:
	// Source Collection
	Collection* collection_;
	// Indices of first and last DisplayDataSet to use in fitting
	int displayDataSetStart_, displayDataSetEnd_;
	// Number of sequential DisplayDataSet to use in fitting
	int nDataSets_;
	// First and last abscissa indices to use in fitting
	int abscissaStart_, abscissaEnd_;
	// Number of sequential abscissa values to use in fitting
	int nPoints_;

	public:
	// Set target information
	void set(Collection* collection, int firstDataSet, int lastDataSet, int abscissaFirst, int abscissaLast);
	// Return index of first DisplayDataSet to be fit
	int displayDataSetStart();
	// Return index of last DisplayDataSet to be fit
	int displayDataSetEnd();
	// Return number of sequential DisplayDataSets to use in fitting
	int nDataSets();
	// Return first abscissa index to use in fitting
	int abscissaStart();
	// Return last abscissa index to use in fitting
	int abscissaEnd();
	// Return number of sequential abscissa values to use in fitting
	int nPoints();
	// Return z value of first dataset used in fitting
	double zStart();
	// Return z value of last dataset used in fitting
	double zEnd();


	/*
	 * Fitting
	 */
	private:
	// Fitted/calculated values for target
	List<Data2D> calculatedY_;

	public:
	// Calculate 'fitted' values from specified equation
	bool calculateY(Tree& equation, Variable* xVariable, Variable* zVariable);
	// Return sos error between calculated and original y values
	double sosError();
	// Copy calculated Y data to specified Collection
	void copyCalculatedY(Collection* target);
};

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


	/*
	 * Equation and Variable Data
	 */
	private:
	// Local list of variables use in equations, including limits
	List<EquationVariable> equationVariables_;
	// Number of variables which are used in the current equation
	int nVariablesUsed_;
	// Original text passed for last equation
	QString equationText_;
	// Tree containing equation to use for fitting
	Tree equation_;
	// Whether current equation is valid
	bool equationValid_;
	// Standard x and z variables
	Variable* xVariable_, *zVariable_;
	// Slice reference variables
	Variable* referenceYVariable_;
	// List of variables targetted in fit process
	RefList<EquationVariable,bool> fitVariables_;
	// Strength of variable limits
	double limitStrength_;

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
	// Return number of variables used in equation
	int nVariablesUsed();
	// Return first variable in list
	EquationVariable* equationVariables();
	// Return first variable used in equation
	RefListItem<EquationVariable,bool>* fitVariables();
	// Return named variable, if it exists
	EquationVariable* variable(QString name);
	// Set strength of variable limits
	void setLimitStrength(double strength);
	// Return strength of variable limits
	double limitStrength();


	/*
	 * Fit Targets
	 */
	public:
	// Range Type enum
	enum RangeType { AbsoluteRange, SinglePointRange, IndexRange, nRangeTypes };
	// Convert text string to RangeType
	static FitKernel::RangeType rangeType(const char* s);
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
	// Data targets to fit, generated from specified ranges
	List<FitTarget> fitTargets_;
	// Current target for fitting
	FitTarget* currentFitTarget_;
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

	private:
	// Update destination with current fitted data
	void updateFittedData();
	// Calculate SOS error for current targets
	double sosError(Array<double>& alpha);
	// Calculate RMS error for current targets
	double rmsError(Array<double>& alpha);
	// Perform fitting with current settings
	bool doFitting();
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
};

#endif
