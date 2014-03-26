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

// Forward Declarations
class UChromaWindow;

/*
 * Fit Variable
 */
class FitVariable : public ListItem<FitVariable>
{
	public:
	// Constructor / Destructor
	FitVariable();
	~FitVariable();

	/*
	 * Variable Target
	 */
	private:
	// Name of target variable
	Dnchar name_;
	// Target Variable pointer
	Variable* variable_;
	// Value
	double value_;
	// Whether minimum maximum limits are enabled
	bool minimumLimitEnabled_, maximumLimitEnabled_;
	// Values for minimum / maximum limits
	double minimumLimit_, maximumLimit_;
	// Whether the variable is to be fit
	bool fit_;
	// Whether the variable is used in the current equation
	bool used_;

	public:
	// Set name
	void setName(const char* name);
	// Return name
	const char* name();
	// Set variable target
	void setVariable(Variable* variable);
	// Return variable target
	Variable* variable();
	// Set value
	void setValue(double value);
	// Return value
	double value();
	// Set minimum limit
	void setMinimumLimit(bool enabled, double value);
	// Return whether minimum limit is enabled
	bool minimumLimitEnabled();
	// Return minimum limit value
	double minimumLimit();
	// Set maximum limit
	void setMaximumLimit(bool enabled, double value);
	// Return whether maximum limit is enabled
	bool maximumLimitEnabled();
	// Return maximum limit value
	double maximumLimit();
	// Set whether this variable should be fit
	void setFit(bool fit);
	// Return whether this variable should be fit
	bool fit();
	// Set whether this variable is used in the current equation
	void setUsed(bool used);
	// Return whether this variable is used in the current equation
	bool used();
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
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;


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
	 * Fit Data
	 */
	private:
	// Local list of fit variables, including limits
	List<FitVariable> fitVariables_;
	// Number of FitVariables which are used in the current equation
	int nFitVariablesUsed_;
	// Tree containing equation to use for fitting
	Tree equation_;
	// Whether current equation is valid
	bool equationValid_;

	private:
	// Reset equation
	void resetEquation();
	// Update fit variables list
	void updateFitVariables();


	/*
	 * Slots / Reimplementations
	 */
	private:
	// Update variable table
	void updateVariableTable();

	public slots:
	void on_EquationEdit_textChanged(QString text);
	void on_SelectEquationButton_clicked(bool checked);
	void on_VariablesTable_cellChanged(int row, int column);
	
	public:
};

#endif
