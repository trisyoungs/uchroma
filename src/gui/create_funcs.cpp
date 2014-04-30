/*
	*** Create Widget Functions
	*** src/gui/create_funcs.cpp
	Copyright T. Youngs 2012-2013

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
#include "parser/scopenode.h"
#include "templates/variantpointer.h"

// Static singletons
UChromaWindow* CreateDialog::uChroma_ = NULL;

// Constructor
CreateDialog::CreateDialog(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);

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
	newCollection_ = NULL;
	resetEquation();
	ui.EquationEdit->setText("cos(x)*A");
	equationValid_ = equation_.setCommands("cos(x)*A");
	updateVariables();
}

// Destructor
CreateDialog::~CreateDialog()
{
}

/*
 * Link to UChroma
 */


// Set UChromaWindow pointer
void CreateDialog::setUChroma(UChromaWindow* ptr)
{
	uChroma_ = ptr;
}

/*
 * Window Functions
 */

// Refresh main view (if options permit) after fit params have changed
void CreateDialog::updateMainView()
{
	if (ui.OptionsVisualCheck->isChecked())
	{
		// Generate updated data, and refresh the main view
		updateCreatedData();
		uChroma_->updateDisplay();
	}
}

// Update data in window
void CreateDialog::updateAndShow()
{
	// Create a new collection for use by the window
	newCollection_ = uChroma_->addCollection("New Creation");
	updateCreatedData();

	updateGridGroup();
	updateVariableTable();
	updateMainView();

	show();
}

/*
 * Creation Data
 */

// Reset equation
void CreateDialog::resetEquation()
{
	equation_.clear();
	xVariable_ = equation_.addGlobalVariable("x");
	zVariable_ = equation_.addGlobalVariable("z");
	equation_.setGenerateMissingVariables(true);
	equationValid_ = false;
}

// Update variables list
void CreateDialog::updateVariables()
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
	ScopeNode* rootNode = equation_.rootNode();
	for (Variable* var = rootNode->variables.variables(); var != NULL; var = var->next)
	{
		// Is this variable one of 'x' or 'z'?
		if ((strcmp(var->name(),"x") == 0) || (strcmp(var->name(),"z") == 0)) continue;

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

// Update created data
void CreateDialog::updateCreatedData(bool force)
{
	// Unless 'force == true', obey the current status of the Visual checkbox
	if ((!force) && (!ui.OptionsVisualCheck->isChecked())) return;

	// First, check destination collection
	if (newCollection_ == NULL)
	{
		msg.print("Internal Error: No valid destination collection pointer in CreateDialog::updateCreatedData().\n");
		return;
	}

	// Set initial equation variables...
	for (RefListItem<EquationVariable,bool>* ri = usedVariables_.first(); ri != NULL; ri = ri->next) ri->item->pokeValueToVariable();

	// Generate new data...
	if (ui.GridSpecifyRadio->isChecked())
	{
		// A grid has been specified by hand, so can clear all old data and recreate it
		newCollection_->clearSlices();
		double z = ui.GridSpecifyZMinSpin->value();
		while (z <= ui.GridSpecifyZMaxSpin->value())
		{
			// Create new slice at this z
			Slice* newSlice = newCollection_->addSlice();
			newCollection_->setSliceZ(newSlice, z);
			Data2D& newData = newSlice->data();

			double x = ui.GridSpecifyXMinSpin->value();
			while (x <= ui.GridSpecifyXMaxSpin->value())
			{
				// Set equation variables
				xVariable_->set(x);
				zVariable_->set(z);
				newData.addPoint(x, equation_.execute());

				x += ui.GridSpecifyXDeltaSpin->value();
			}

			z += ui.GridSpecifyZDeltaSpin->value();
		}
	}
	else
	{
	}

	// Recalculate new data ranges
	newCollection_->calculateDataLimits();

	// Update main window
	uChroma_->updateAllTabs();
	uChroma_->updateDisplay();
}

/*
 * Widgets / Slots / Reimplementations
 */

void CreateDialog::on_CloseButton_clicked(bool checked)
{
	// Must remove collection pointed to by newCollection_, if it exists
	if (newCollection_ != NULL) uChroma_->removeCollection(newCollection_);
	newCollection_ = NULL;

	uChroma_->updateAllTabs();

	hide();
}

void CreateDialog::on_CreateButton_clicked(bool checked)
{
	// Make sure the data is up to date
	updateCreatedData(true);

	uChroma_->setAsModified();

	// Set the title of the current collection to the current equation text
	newCollection_->setTitle(ui.EquationEdit->text());

	// Create another new collection for the next creation...
	newCollection_ = uChroma_->addCollection("New Creation");

	uChroma_->updateAllTabs();
}

/*
 * Equations Group
 */

void CreateDialog::on_EquationEdit_textChanged(QString text)
{
	resetEquation();
	equationValid_ = equation_.setCommands(text);
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

	// Update data
	updateCreatedData();
}

void CreateDialog::on_SelectEquationButton_clicked(bool checked)
{
}

/*
 * Grid Group
 */

// Update grid data group
void CreateDialog::updateGridGroup(bool refreshList)
{
	refreshing_ = true;

	// Update 'take from' collection combo
	if (refreshList)
	{
		int currentIndex = ui.GridTakeFromCollectionCombo->currentIndex();
		ui.GridTakeFromCollectionCombo->clear();
		for (Collection* c = uChroma_->collections(); c != NULL; c = c->next) ui.GridTakeFromCollectionCombo->addItem(c->title());
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

void CreateDialog::on_GridSpecifyXMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateDialog::on_GridSpecifyXMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateDialog::on_GridSpecifyXDeltaSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateDialog::on_GridSpecifyZMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateDialog::on_GridSpecifyZMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateDialog::on_GridSpecifyZDeltaSpin_valueChanged(double value)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

void CreateDialog::on_GridTakeFromCollectionCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	updateGridGroup(false);
}

/*
 * Variables Group
 */

// Update variable table
void CreateDialog::updateVariableTable()
{
	refreshing_ = true;

	// Get rootnode of the equation
	ScopeNode* rootNode = equation_.rootNode();

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
	ReturnValue rv;
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

void CreateDialog::on_VariablesTable_cellChanged(int row, int column)
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
