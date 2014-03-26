/*
	*** Fit Widget Functions
	*** src/gui/fit_funcs.cpp
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

#include "gui/fit.h"
#include "gui/uchroma.h"
#include <parser/scopenode.h>

// Static singletons
UChromaWindow* FitDialog::uChroma_ = NULL;

/*
 * Fit Variable
 */

// Constructor
FitVariable::FitVariable() : ListItem<FitVariable>()
{
	variable_ = NULL;
	value_ = 10203.4;
	minimumLimitEnabled_ = false;
	maximumLimitEnabled_ = false;
	minimumLimit_ = -10.0;
	maximumLimit_ = 10.0;
	used_ = false;
	fit_ = true;
}

// Destructor
FitVariable::~FitVariable()
{
}

// Set name
void FitVariable::setName(const char* name)
{
	name_ = name;
}

// Return name
const char* FitVariable::name()
{
	return name_.get();
}

// Set variable target
void FitVariable::setVariable(Variable* variable)
{
	variable_ = variable;
}

// Return variable target
Variable* FitVariable::variable()
{
	return variable_;
}

// Set initial value
void FitVariable::setValue(double value)
{
	value_ = value;
}

// Return initial value
double FitVariable::value()
{
	return value_;
}

// Set minimum limit
void FitVariable::setMinimumLimit(bool enabled, double value)
{
	minimumLimit_ = value;
	minimumLimitEnabled_ = enabled;
}

// Return whether minimum limit is enabled
bool FitVariable::minimumLimitEnabled()
{
	return minimumLimitEnabled_;
}

// Return minimum limit value
double FitVariable::minimumLimit()
{
	return minimumLimit_;
}

// Set maximum limit
void FitVariable::setMaximumLimit(bool enabled, double value)
{
	maximumLimit_ = value;
	maximumLimitEnabled_ = enabled;
}

// Return whether maximum limit is enabled
bool FitVariable::maximumLimitEnabled()
{
	return maximumLimitEnabled_;
}

// Return maximum limit value
double FitVariable::maximumLimit()
{
	return maximumLimit_;
}

// Set whether this variable should be fit
void FitVariable::setFit(bool fit)
{
	fit_ = fit;
}

// Return whether this variable should be fit
bool FitVariable::fit()
{
	return fit_;
}

// Set whether this variable is used in the current equation
void FitVariable::setUsed(bool used)
{
	used_ = used;
}

// Return whether this variable is used in the current equation
bool FitVariable::used()
{
	return used_;
}

/*
 * Fit Dialog
 */

// Constructor
FitDialog::FitDialog(QWidget* parent) : QDialog(parent)
{
	refreshing_ = false;
	resetEquation();
	updateFitVariables();
	ui.setupUi(this);
}

// Destructor
FitDialog::~FitDialog()
{
}

/*
 * Link to UChroma
 */

// Set UChromaWindow pointer
void FitDialog::setUChroma(UChromaWindow* ptr)
{
	uChroma_ = ptr;
}

/*
 * Fit Data
 */

// Reset equation
void FitDialog::resetEquation()
{
	equation_.clear();
	equation_.addGlobalVariable("x");
	equation_.addGlobalVariable("y");
	equation_.addGlobalVariable("z");
	equation_.setGenerateMissingVariables(true);
	equationValid_ = false;
}

// Update fit variables list
void FitDialog::updateFitVariables()
{
	// First, clear all 'used' flags
	FitVariable* fitVar;
	for (fitVar = fitVariables_.first(); fitVar != NULL; fitVar = fitVar->next)
	{
		fitVar->setVariable(NULL);
		fitVar->setUsed(false);
	}
	nFitVariablesUsed_ = 0;

	// Now, loop over current variables in the equation_
	// Ignore 'x', 'y', and 'z' if they exist
	// If a variable already exists in fitVariables_, set it's 'used' status to true.
	// If it doesn't, create it and set it's 'used' status to true
	ScopeNode* rootNode = equation_.rootNode();
	for (Variable* var = rootNode->variables.variables(); var != NULL; var = var->next)
	{
		// Is this variable one of 'x', 'y', or 'z'?
		if (strcmp(var->name(),"x") == 0 || strcmp(var->name(),"y") == 0 || strcmp(var->name(),"z") == 0) continue;

		for (fitVar = fitVariables_.first(); fitVar != NULL; fitVar = fitVar->next) if (strcmp(fitVar->name(),var->name()) == 0) break;
		if (fitVar == NULL)
		{
			fitVar = fitVariables_.add();
			fitVar->setName(var->name());
		}

		// Update variable pointer
		fitVar->setVariable(var);
		fitVar->setUsed(true);
		++nFitVariablesUsed_;
	}
}

/*
// Widgets / Slots / Reimplementations
*/

// Update variable table
void FitDialog::updateVariableTable()
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
	ui.VariablesTable->setRowCount(nFitVariablesUsed_);

	QTableWidgetItem* item;
	bool status, isXYZ;
	ReturnValue rv;
	int count = 0, n;
	for (int n=0; n<fitVariables_.nItems(); ++n)
	{
		// Grab the fit variable and see if it's used in the current equation?
		FitVariable* fitVar = fitVariables_[n];
		if (!fitVar->used()) continue;

		// Grab variable pointer from FitVariable
		Variable* var = fitVar->variable();

		// -- Variable Name / Fit flag
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
		item->setCheckState(fitVar->fit() ? Qt::Checked : Qt::Unchecked);
		item->setText(var->name());
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 0, item);

		// -- Value
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		item->setText(QString::number(fitVar->value()));
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 1, item);

		// -- Minimum limit
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		item->setCheckState(fitVar->minimumLimitEnabled() ? Qt::Checked : Qt::Unchecked);
		item->setText(QString::number(fitVar->minimumLimit()));
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 2, item);

		// -- Maximum limit
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		item->setCheckState(fitVar->maximumLimitEnabled() ? Qt::Checked : Qt::Unchecked);
		item->setText(QString::number(fitVar->maximumLimit()));
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 3, item);
		
		++count;
	}

	refreshing_ = false;
}

void FitDialog::on_EquationEdit_textChanged(QString text)
{
	resetEquation();
	equationValid_= equation_.setCommands(text);
	if (equationValid_)
	{
		ui.EquationEdit->setPalette(ui.EquationGroup->palette());
	}
	else
	{
		QPalette redText;
		redText.setColor(QPalette::Text, Qt::red);
		ui.EquationEdit->setPalette(redText);
	}

	// Update list of variables
	updateFitVariables();
	updateVariableTable();
}

void FitDialog::on_SelectEquationButton_clicked(bool checked)
{
}

void FitDialog::on_VariablesTable_cellChanged(int row, int column)
{
	if (refreshing_) return;

	// The index of the associated FirVariable will have been stored as the item's data
	QTableWidgetItem* item = ui.VariablesTable->item(row, column);
	if (!item) return;

	// Get associated data
	int index = item->data(Qt::UserRole).toInt();
	FitVariable* fitVariable = fitVariables_[index];
	if (!fitVariable) return;

	// Our action depends on the column edited...
	switch (column)
	{
		// Variable name / fit flag
		case (0):
			fitVariable->setFit(item->checkState() == Qt::Checked);
			break;
		// Variable value
		case (1):
			fitVariable->setValue(item->text().toDouble());
			break;
		// Minimum value and limit flag
		case (2):
			fitVariable->setMinimumLimit(item->checkState() == Qt::Checked, item->text().toDouble());
			break;
		// Maximum value and limit flag
		case (3):
			fitVariable->setMaximumLimit(item->checkState() == Qt::Checked, item->text().toDouble());
			break;
	}
}
