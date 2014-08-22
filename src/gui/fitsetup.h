/*
	*** Fit Setup Dialog
	*** src/gui/fitsetup.h
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

#ifndef UCHROMA_FITSETUP_H
#define UCHROMA_FITSETUP_H

#include <QtGui/QDialog>
#include <QtCore/QObject>
#include "gui/ui_fitsetup.h"
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
class FitKernel;

/*
 * FitSetup Dialog
 */
class FitSetupDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	FitSetupDialog(UChromaWindow& parent);
	// Destructor
	~FitSetupDialog();
	// Main form declaration
	Ui::FitSetupDialog ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public slots:
	void on_CloseButton_clicked(bool checked);


	/*
	 * Fitting Data Setup
	 */
	private:
	// FitKernel that we are currently setting up
	FitKernel* fitKernel_;

	public:
	// Set new FitKernel target
	bool setFitKernel(FitKernel* fitKernel);


	/*
	 * Equation Group
	 */
	public slots:
	void on_EquationEdit_textChanged(QString text);
	void on_SelectEquationButton_clicked(bool checked);


	/*
	 * Variables Group
	 */
	private:
	// Update variable table
	void updateVariableTable();

	public slots:
	void on_VariablesTable_cellChanged(int row, int column);
	void on_VariablesLimitStrengthSpin_valueChanged(double value);


	/*
	 * Source Data
	 */
	public slots:
	void on_DataNormalFitRadio_clicked(bool checked);
	void on_DataOrthogonalFitRadio_clicked(bool checked);
	void on_DataGlobalFit_clicked(bool checked);


	/*
	 * Minimisation
	 */
	public slots:
	void on_MinimisationMethodCombo_currentIndexChanged(int index);
	void on_MinimisationToleranceSpin_valueChanged(double value);
	void on_MinimisationMaxStepsSpin_valueChanged(int value);


	/*
	 * Source X
	 */
	public slots:
	void on_XSourceAbsoluteRadio_clicked(bool checked);
	void on_XAbsoluteMinSpin_valueChanged(double value);
	void on_XAbsoluteMaxSpin_valueChanged(double value);
	void on_XAbsoluteSelectButton_clicked(bool checked);
	void on_XSourceSinglePointRadio_clicked(bool checked);
	void on_XPointSingleSpin_valueChanged(int value);
	void on_XSourcePointRangeRadio_clicked(bool checked);
	void on_XPointMinSpin_valueChanged(int value);
	void on_XPointMaxSpin_valueChanged(int value);


	/*
	 * Source Z
	 */
	public slots:
	void on_ZSourceAbsoluteRadio_clicked(bool checked);
	void on_ZAbsoluteMinSpin_valueChanged(double value);
	void on_ZAbsoluteMaxSpin_valueChanged(double value);
	void on_ZAbsoluteSelectButton_clicked(bool checked);
	void on_ZSourceSingleDataSetRadio_clicked(bool checked);
	void on_ZDataSetCombo_currentIndexChanged(int index);
	void on_ZSourceDataSetRangeRadio_clicked(bool checked);
	void on_ZDataSetMinSpin_valueChanged(int value);
	void on_ZDataSetMaxSpin_valueChanged(int value);


	/*
	 * Update Functions
	 */
	public:
	// Update controls and show window
	void updateAndShow();
	// Update controls
	void updateControls(bool force = false);
	// Update labels
	void updateLabels();
};

#endif
