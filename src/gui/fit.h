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
	bool calculateY(Tree& equation, Variable* xVariable, Variable* zVariable, Variable* referenceYVariable, int referenceYIndex);
	// Return sos error between calculated and original y values
	double sosError();
	// Copy calculated Y data to specified Collection
	void copyCalculatedY(Collection* target);
};

/*
 * Fit Window
 */
class FitWindow : public QWidget
{
	Q_OBJECT

	public:
	// Constructor
	FitWindow(UChromaWindow& parent);
	// Destructor
	~FitWindow();
	// Main form declaration
	Ui::FitWindow ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;


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

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	signals:
	// Window closed signal
	void windowClosed(bool);


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
	// Source collection for fitting
	Collection* sourceCollection_;
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

	public:
	// Set source collection
	void setSourceCollection(Collection* collection);


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
	// Update source data
	void updateSourceData(bool setInitialValues = false);

	public slots:
	void on_SourceDataSetFromSpin_valueChanged(int value);
	void on_SourceDataSetToSpin_valueChanged(int value);
	void on_SourceXSelectButton_clicked(bool checked);


	/*
	 * Destination Group
	 */
	private:
	// Update destination data group
	void updateDestinationGroup();


	/*
	 * Update Functions
	 */
	public:
	// Update controls and show window
	void updateAndShow();
	// Update controls
	void updateControls(bool force = false);
};

#endif
