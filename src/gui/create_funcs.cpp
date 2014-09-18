/*
	*** Create Widget Functions
	*** src/gui/create_funcs.cpp
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

#include "gui/create.h"
#include "gui/uchroma.h"
#include "gui/selectequation.h"
#include "expression/variable.h"
#include "base/session.h"

// Constructor
CreateCollectionDialog::CreateCollectionDialog(UChromaWindow& parent) : QDialog(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	// Create static x and z variables
	xVariable_ = equation_.createVariable("x", NULL, true);
	zVariable_ = equation_.createVariable("z", NULL, true);
	equation_.setGenerateMissingVariables(true);

	// Set default values in some widgets
	ui.GridSpecifyXMinSpin->setValue(0.0);
	ui.GridSpecifyXMaxSpin->setValue(10.0);
	ui.GridSpecifyXDeltaSpin->setValue(0.1);
	ui.GridSpecifyXDeltaSpin->setMinimumValue(1.0e-5);
	ui.GridSpecifyZMinSpin->setValue(0.0);
	ui.GridSpecifyZMaxSpin->setValue(10.0);
	ui.GridSpecifyZDeltaSpin->setValue(0.1);
	ui.GridSpecifyZDeltaSpin->setMinimumValue(1.0e-5);

	refreshing_ = false;
	resetEquation();
	ui.EquationEdit->setText("cos(x)*A");
	equationValid_ = equation_.generate("cos(x)*A");
	updateVariables();
}

// Destructor
CreateCollectionDialog::~CreateCollectionDialog()
{
}

/*
 * Window Functions
 */

// Update data in window
void CreateCollectionDialog::updateAndShow()
{
	updateGridGroup();
	updateVariableTable();

	show();
}

/*
 * Creation Data
 */

// Reset equation
void CreateCollectionDialog::resetEquation()
{
	equation_.clear();
	equationValid_ = false;
}

// Update variables list
void CreateCollectionDialog::updateVariables()
{
	// First, clear all 'used' flags
	EquationVariable* eqVar;
	for (eqVar = equationVariables_.first(); eqVar != NULL; eqVar = eqVar->next)
	{
		eqVar->setVariable(NULL);
		eqVar->setUsed(false);
	}
	nVariablesUsed_ = 0;
	usedVariables_.clear();

	// Now, loop over current variables in the equation_
	// Ignore 'x' and 'z' if they exist
	// If a variable already exists in equationVariables_, set it's 'used' status to true.
	// If it doesn't, create it and set it's 'used' status to true
	for (RefListItem<Variable,bool>* ri = equation_.variables(); ri != NULL; ri = ri->next)
	{
		Variable* var = ri->item;

		// Is this variable one of 'x' or 'z'?
		if ((var->name() == "x") || (var->name() == "z")) continue;

		for (eqVar = equationVariables_.first(); eqVar != NULL; eqVar = eqVar->next) if (eqVar->name() == var->name()) break;
		if (eqVar == NULL)
		{
			eqVar = equationVariables_.add();
			eqVar->setName(var->name());
		}

		// Update variable pointer
		eqVar->setVariable(var);
		eqVar->setUsed(true);
		usedVariables_.add(eqVar);
		++nVariablesUsed_;
	}
}

// Create data
void CreateCollectionDialog::createData(Collection* target)
{
	bool success;

	// First, check equation status
	if (!equationValid_) return;

	// Second, check destination collection
	if (target == NULL)
	{
		msg.print("Internal Error: No valid destination collection pointer in CreateCollectionDialog::updateCreatedData().\n");
		return;
	}

	// Set initial equation variables...
	for (RefListItem<EquationVariable,bool>* ri = usedVariables_.first(); ri != NULL; ri = ri->next) ri->item->pokeValueToVariable();

	// Generate new data...
	if (ui.GridSpecifyRadio->isChecked())
	{
		// A grid has been specified by hand, so can clear all old data and recreate it
		target->clearDataSets();
		double z = ui.GridSpecifyZMinSpin->value();
		while (z <= ui.GridSpecifyZMaxSpin->value())
		{
			// Create new dataset at this z
			DataSet* newDataSet = target->addDataSet();
			target->setDataSetZ(newDataSet, z);
			Data2D& newData = newDataSet->data();

			double x = ui.GridSpecifyXMinSpin->value();
			while (x <= ui.GridSpecifyXMaxSpin->value())
			{
				// Set equation variables
				xVariable_->set(x);
				zVariable_->set(z);
				newData.addPoint(x, equation_.execute(success));

				x += ui.GridSpecifyXDeltaSpin->value();
			}

			z += ui.GridSpecifyZDeltaSpin->value();
		}
	}
	else
	{
		// TODO
	}
}

/*
 * Widgets / Slots / Reimplementations
 */

void CreateCollectionDialog::on_CancelButton_clicked(bool checked)
{
	uChroma_.updateSubWindows();

	reject();
}

void CreateCollectionDialog::on_CreateButton_clicked(bool checked)
{
	// Create a new collection
	Collection* target = uChroma_.addCollection(ui.EquationEdit->text());
	if (uChroma_.currentViewPane()) uChroma_.currentViewPane()->addCollection(target);
	
	// Make sure the data is up to date
	createData(target);

	Session::setAsModified();

	uChroma_.updateGUI();
}

/*
 * Equations Group
 */

void CreateCollectionDialog::on_EquationEdit_textChanged(QString text)
{
	resetEquation();
	equationValid_ = equation_.generate(qPrintable(text), true);
	if (equationValid_)
	{
		ui.EquationEdit->setPalette(ui.EquationGroup->palette());
		ui.CreateButton->setEnabled(true);
	}
	else
	{
		QPalette redText;
		redText.setColor(QPalette::Text, Qt::red);
		ui.EquationEdit->setPalette(redText);
		ui.CreateButton->setEnabled(false);
	}

	// Update list of variables
	updateVariables();
	updateVariableTable();
}

void CreateCollectionDialog::on_EquationSelectButton_clicked(bool checked)
{
	SelectEquationDialog equationDialog(this);
	if (equationDialog.exec()) ui.EquationEdit->setText(equationDialog.selectedEquation().equationText);
}

/*
 * Grid Group
 */

// Update grid data group
void CreateCollectionDialog::updateGridGroup(bool refreshList)
{
	refreshing_ = true;

	// Update 'take from' collection combo
	if (refreshList)
	{
		int currentIndex = ui.GridTakeFromCollectionCombo->currentIndex();
		ui.GridTakeFromCollectionCombo->clear();
		for (Collection* c = uChroma_.collections(); c != NULL; c = c->next) ui.GridTakeFromCollectionCombo->addItem(c->name());
		if ((currentIndex != -1) && (currentIndex < ui.GridTakeFromCollectionCombo->count())) ui.GridTakeFromCollectionCombo->setCurrentIndex(currentIndex);
		else ui.GridTakeFromCollectionCombo->setCurrentIndex(0);
	}

	// Set values in spin boxes (if taking from collection)
	if (ui.GridTakeFromRadio->isChecked())
	{
	}

	// Set limits on spin boxes
	ui.GridSpecifyXMaxSpin->setMinimumValue(ui.GridSpecifyXMinSpin->value());
	ui.GridSpecifyXMinSpin->setMaximumValue(ui.GridSpecifyXMaxSpin->value());
	ui.GridSpecifyZMaxSpin->setMinimumValue(ui.GridSpecifyZMinSpin->value());
	ui.GridSpecifyZMinSpin->setMaximumValue(ui.GridSpecifyZMaxSpin->value());

	// Calculate number of points that will occur in generated data, for nPoints label
	int nx = (int) floor((ui.GridSpecifyXMaxSpin->value() - ui.GridSpecifyXMinSpin->value()) / ui.GridSpecifyXDeltaSpin->value());
	int nz = (int) floor((ui.GridSpecifyZMaxSpin->value() - ui.GridSpecifyZMinSpin->value()) / ui.GridSpecifyZDeltaSpin->value());
	QString s;
	s.sprintf("NPoints = %i (%i x %i)", nx*nz, nx, nz);
	ui.GridNPointsLabel->setText(s);

	refreshing_ = false;
}

void CreateCollectionDialog::on_GridSpecifyXMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateCollectionDialog::on_GridSpecifyXMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateCollectionDialog::on_GridSpecifyXDeltaSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateCollectionDialog::on_GridSpecifyZMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateCollectionDialog::on_GridSpecifyZMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateCollectionDialog::on_GridSpecifyZDeltaSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateCollectionDialog::on_GridTakeFromCollectionCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

/*
 * Variables Group
 */

// Update variable table
void CreateCollectionDialog::updateVariableTable()
{
	refreshing_ = true;

	// Clear table, but don't display anything if equation is not valid
	ui.VariablesTable->clearContents();
	if (!equationValid_)
	{
		refreshing_ = false;
		return;
	}
	ui.VariablesTable->setRowCount(nVariablesUsed_);

	QTableWidgetItem* item;
	bool status, isXYZ;
	int count = 0, n;
	for (int n=0; n<equationVariables_.nItems(); ++n)
	{
		// Grab the variable and see if it's used in the current equation?
		EquationVariable* eqVar = equationVariables_[n];
		if (!eqVar->used()) continue;

		// Grab variable pointer from FitVariable
		Variable* var = eqVar->variable();

		// -- Variable Name / Fit flag
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		item->setText(var->name());
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 0, item);

		// -- Value
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		item->setText(QString::number(eqVar->value()));
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 1, item);

		++count;
	}

	refreshing_ = false;
}

void CreateCollectionDialog::on_VariablesTable_cellChanged(int row, int column)
{
	if (refreshing_) return;

	// The index of the associated FiVariable will have been stored as the item's data
	QTableWidgetItem* item = ui.VariablesTable->item(row, column);
	if (!item) return;

	// Get associated data
	int index = item->data(Qt::UserRole).toInt();
	EquationVariable* eqVariable = equationVariables_[index];
	if (!eqVariable) return;

	// Our action depends on the column edited...
	switch (column)
	{
		// Variable value
		case (1):
			eqVariable->setValue(item->text().toDouble());
			break;
	}
}
