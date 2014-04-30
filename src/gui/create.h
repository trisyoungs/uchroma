/*
	*** Create Widget
	*** src/gui/create.h
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

#ifndef UCHROMA_CREATE_H
#define UCHROMA_CREATE_H

#include <QtGui/QWidget>
#include <QtCore/QObject>
#include "gui/ui_create.h"
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
 * Create Dialog
 */
class CreateDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	CreateDialog(QWidget *parent);
	// Destructor
	~CreateDialog();
	// Main form declaration
	Ui::CreateDialog ui;


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
	// Refresh main view (if options permit) after fit params have changed
	void updateMainView();

	public:
	// Update data and show window
	void updateAndShow();


	/*
	 * Creation Data
	 */
	private:
	// Local list of variables use in equations
	List<EquationVariable> equationVariables_;
	// Number of variables which are used in the current equation
	int nVariablesUsed_;
	// Tree containing equation to use for fitting
	Tree equation_;
	// Whether current equation is valid
	bool equationValid_;
	// Standard x and z variables
	Variable* xVariable_, *zVariable_;
	// List of variables targetted in create process
	RefList<EquationVariable,bool> usedVariables_;
	// Newly created collection (if any)
	Collection* newCollection_;

	private:
	// Reset equation
	void resetEquation();
	// Update variables list
	void updateVariables();
	// Update created data
	void updateCreatedData(bool force = false);


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
	void on_CreateButton_clicked(bool checked);

	/*
	 * Grid Group
	 */
	private:
	// Update grid data group
	void updateGridGroup(bool refreshList = true);

	public slots:
	void on_GridSpecifyXMinSpin_valueChanged(double value);
	void on_GridSpecifyXMaxSpin_valueChanged(double value);
	void on_GridSpecifyXDeltaSpin_valueChanged(double value);
	void on_GridSpecifyZMinSpin_valueChanged(double value);
	void on_GridSpecifyZMaxSpin_valueChanged(double value);
	void on_GridSpecifyZDeltaSpin_valueChanged(double value);
	void on_GridTakeFromCollectionCombo_currentIndexChanged(int index);

	/*
	 * Variables Group
	 */
	private:
	// Update variable table
	void updateVariableTable();

	public slots:
	void on_VariablesTable_cellChanged(int row, int column);
};

#endif
