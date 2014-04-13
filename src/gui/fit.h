/*
	*** Fit Widget
	*** src/gui/fit.h
	Copyright T. Youngs 2012-2013.

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
class Variable;

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
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Update data in window
	void updateAll();


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
	// Standard x, y, and z variables
	Variable* xVariable_, *yVariable_, *zVariable_;
	// Data to fit
	List<Data2D> fitData_;
	// Current fitted datapoints
	List<Data2D> fittedData_;
	// Current Data2D to fit, or NULL for all Data2D simultaneously
	Data2D* currentFitData_;
	// Current fitted Data2D, or NULL if fitting all simultaneously
	Data2D* currentFittedData_;
	// List of variables targetted in fit process
	RefList<EquationVariable,bool> fitVariables_;

	private:
	// Reset equation
	void resetEquation();
	// Update fit variables list
	void updateFitVariables();
	// Generate fitted data for current targets
	bool generateFittedData();
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
	bool sdMinimise(Array<double>& alpha);
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
