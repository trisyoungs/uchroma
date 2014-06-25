/*
	*** Fit Widget
	*** src/gui/fit.h
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

#include <QtGui/QWidget>
#include <QtCore/QObject>
#include "gui/ui_fit.h"
#include "base/dnchar.h"
#include "base/slice.h"
#include "base/equation.h"
#include "parser/tree.h"
#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
class UChromaWindow;
class Collection;
class Slice;
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
	// Indices of first and last DisplaySlice to use in fitting
	int displaySliceStart_, displaySliceEnd_;
	// Number of sequential DisplaySlices to use in fitting
	int nSlices_;
	// First and last abscissa indices to use in fitting
	int abscissaStart_, abscissaEnd_;
	// Number of sequential abscissa values to use in fitting
	int nPoints_;

	public:
	// Set target information
	void set(Collection* collection, int firstSlice, int lastSlice, int abscissaFirst, int abscissaLast);
	// Return index of first DisplaySice to be fit
	int displaySliceStart();
	// Return index of last DisplaySice to be fit
	int displaySliceEnd();
	// Return number of sequential DisplaySlices to use in fitting
	int nSlices();
	// Return first abscissa index to use in fitting
	int abscissaStart();
	// Return last abscissa index to use in fitting
	int abscissaEnd();
	// Return number of sequential abscissa values to use in fitting
	int nPoints();
	// Return z value of first slice used in fitting
	double zStart();
	// Return z value of last slice used in fitting
	double zEnd();


	/*
	 * Fitting
	 */
	private:
	// Fitted/calculated values for target
	List<Data2D> calculatedY_;

	public:
	// Calculate 'fitted' values from specified equation
	bool calculateY(Tree& equation, Variable* xVariable, Variable* zVariable, Variable* referenceYVariable, int referenceYIndex);
	// Return sos error between calculated and original y values
	double sosError();
	// Copy calculated Y data to specified Collection
	void copyCalculatedY(Collection* target);
};

/*
 * Fit Dialog
 */
class FitDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	FitDialog(QWidget *parent);
	// Destructor
	~FitDialog();
	// Main form declaration
	Ui::FitDialog ui;


	/*
	 * Link to UChroma
	 */
	private:
	// UChromaWindow pointer
	static UChromaWindow* uChroma_;
	
	public:
	// Set UChromaWindow pointer
	static void setUChroma(UChromaWindow* ptr);


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	private:
	// Send message to message box
	void printMessage(const char* fmtString, ...) const;
	// Refresh main view (if options permit) after fit params have changed
	void updateMainView();

	public:
	// Update data in window
	void updateAll();


	/*
	 * Main Fitting Data
	 */
	private:
	// Local list of variables use in equations, including limits
	List<EquationVariable> equationVariables_;
	// Number of variables which are used in the current equation
	int nVariablesUsed_;
	// Tree containing equation to use for fitting
	Tree equation_;
	// Whether current equation is valid
	bool equationValid_;
	// Standard x and z variables
	Variable* xVariable_, *zVariable_;
	// Slice reference variables
	Variable* referenceYVariable_;
	// Data targets to fit
	List<FitTarget> fitTargets_;
	// Current target for fitting
	FitTarget* currentFitTarget_;
	// List of variables targetted in fit process
	RefList<EquationVariable,bool> fitVariables_;
	// Destination collection for fitted data
	Collection* destinationCollection_;

	private:
	// Reset equation
	void resetEquation();
	// Update variables list
	void updateVariables();
	// Update destination with current fitted data
	void updateFittedData();
	// Calculate SOS error for current targets
	double sosError(Array<double>& alpha);
	// Calculate RMS error for current targets
	double rmsError(Array<double>& alpha);
	// Perform fitting with current settings
	bool doFitting();


	/*
	 * Minimisation Functions
	 */
	private:
	// Simplex minimise
	bool simplexMinimise(Array<double>& alpha);
	// Steepest Descent minimise
	bool sdMinimise(Array<double>& alpha, double tolerance, int maxSteps);
	// Minimise, calling relevant method
	bool minimise();


	/*
	 * Slots / Reimplementations
	 */
	public slots:
	void on_CloseButton_clicked(bool checked);

	/*
	 * Equation Group
	 */
	public slots:
	void on_EquationEdit_textChanged(QString text);
	void on_SelectEquationButton_clicked(bool checked);
	void on_FitButton_clicked(bool checked);

	/*
	 * Variables Group
	 */
	private:
	// Update variable table
	void updateVariableTable();

	public slots:
	void on_VariablesTable_cellChanged(int row, int column);

	/*
	 * Source Group
	 */
	private:
	// Update source data group
	void updateSourceGroup(bool refreshList = true);

	public slots:
	void on_SourceCollectionCombo_currentIndexChanged(int index);
	void on_SourceXYSliceFromSpin_valueChanged(int value);
	void on_SourceXYSliceToSpin_valueChanged(int value);

	/*
	 * Destination Group
	 */
	private:
	// Update destination data group
	void updateDestinationGroup();

};

#endif
