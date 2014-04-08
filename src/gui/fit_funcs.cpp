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
EquationVariable::EquationVariable() : ListItem<EquationVariable>()
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
EquationVariable::~EquationVariable()
{
}

// Set name
void EquationVariable::setName(const char* name)
{
	name_ = name;
}

// Return name
const char* EquationVariable::name()
{
	return name_.get();
}

// Set variable target
void EquationVariable::setVariable(Variable* variable)
{
	variable_ = variable;
}

// Return variable target
Variable* EquationVariable::variable()
{
	return variable_;
}

// Set initial value
void EquationVariable::setValue(double value)
{
	value_ = value;
}

// Return initial value
double EquationVariable::value()
{
	return value_;
}

// Set minimum limit
void EquationVariable::setMinimumLimit(bool enabled, double value)
{
	minimumLimit_ = value;
	minimumLimitEnabled_ = enabled;
}

// Return whether minimum limit is enabled
bool EquationVariable::minimumLimitEnabled()
{
	return minimumLimitEnabled_;
}

// Return minimum limit value
double EquationVariable::minimumLimit()
{
	return minimumLimit_;
}

// Set maximum limit
void EquationVariable::setMaximumLimit(bool enabled, double value)
{
	maximumLimit_ = value;
	maximumLimitEnabled_ = enabled;
}

// Return whether maximum limit is enabled
bool EquationVariable::maximumLimitEnabled()
{
	return maximumLimitEnabled_;
}

// Return maximum limit value
double EquationVariable::maximumLimit()
{
	return maximumLimit_;
}

// Set whether this variable should be fit
void EquationVariable::setFit(bool fit)
{
	fit_ = fit;
}

// Return whether this variable should be fit
bool EquationVariable::fit()
{
	return fit_;
}

// Set whether this variable is used in the current equation
void EquationVariable::setUsed(bool used)
{
	used_ = used;
}

// Return whether this variable is used in the current equation
bool EquationVariable::used()
{
	return used_;
}

/*
 * Fit Dialog
 */

// Constructor
FitDialog::FitDialog(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);

	refreshing_ = false;
	resetEquation();
	ui.EquationEdit->setText("cos(x)");
	equationValid_ = equation_.setCommands("cos(x)");
	updateFitVariables();
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

// Update data in window
void FitDialog::updateAll()
{
	updateSourceGroup();
	updateDestinationGroup();
}

/*
 * Fit Data
 */

// Reset equation
void FitDialog::resetEquation()
{
	equation_.clear();
	xVariable_ = equation_.addGlobalVariable("x");
	yVariable_ = equation_.addGlobalVariable("y");
	zVariable_ = equation_.addGlobalVariable("z");
	equation_.setGenerateMissingVariables(true);
	equationValid_ = false;
}

// Update fit variables list
void FitDialog::updateFitVariables()
{
	// First, clear all 'used' flags
	EquationVariable* eqVar;
	for (eqVar = equationVariables_.first(); eqVar != NULL; eqVar = eqVar->next)
	{
		eqVar->setVariable(NULL);
		eqVar->setUsed(false);
	}
	nVariablesUsed_ = 0;
	fitVariables_.clear();

	// Now, loop over current variables in the equation_
	// Ignore 'x', 'y', and 'z' if they exist
	// If a variable already exists in equationVariables_, set it's 'used' status to true.
	// If it doesn't, create it and set it's 'used' status to true
	ScopeNode* rootNode = equation_.rootNode();
	for (Variable* var = rootNode->variables.variables(); var != NULL; var = var->next)
	{
		// Is this variable one of 'x', 'y', or 'z'?
		if (strcmp(var->name(),"x") == 0 || strcmp(var->name(),"y") == 0 || strcmp(var->name(),"z") == 0) continue;

		for (eqVar = equationVariables_.first(); eqVar != NULL; eqVar = eqVar->next) if (strcmp(eqVar->name(),var->name()) == 0) break;
		if (eqVar == NULL)
		{
			eqVar = equationVariables_.add();
			eqVar->setName(var->name());
		}

		// Update variable pointer
		eqVar->setVariable(var);
		eqVar->setUsed(true);
		fitVariables_.add(eqVar);
		++nVariablesUsed_;
	}
}

/*
// Widgets / Slots / Reimplementations
*/

/*
 * Equations Group
 */

void FitDialog::on_EquationEdit_textChanged(QString text)
{
	resetEquation();
	equationValid_ = equation_.setCommands(text);
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

void FitDialog::on_FitButton_clicked(bool checked)
{
	// Grab source collection, and construct a list of data to fit, obeying all defined data limits
	fitData_.clear();
	Collection* collection = uChroma_->collection(ui.SourceCollectionCombo->currentIndex());
	if (!collection) return;
	if (ui.SourceXYSlicesRadio->isChecked())
	{
		// Source data is normal XY slices from the current collection
		double xMin = ui.SourceXYXMinSpin->value(), xMax = ui.SourceXYXMaxSpin->value();
		for (int n=ui.SourceXYSliceFromSpin->value(); n<ui.SourceXYSliceToSpin->value(); ++n)
		{
			Data2D& oldData = collection->slice(n)->data();
			Slice* newSlice = fitData_.add();
			newSlice->data().setZ(oldData.z());

			// Copy x-range specified
			for (int m=0; m<oldData.nPoints(); ++m)
			{
				if (oldData.x(m) < xMin) continue;
				if (oldData.x(m) > xMax) break;
				newSlice->data().addPoint(oldData.x(m), oldData.y(m));
			}
		}
	}
	else if (ui.SourceYZSlicesRadio->isChecked())
	{
		// TODO
	}

	if (ui.FitAtOnceCheck->isChecked())
	{
		 currentFitSlice_ = NULL;

		// Call the relevant minimiser
		simplexMinimise();
	}
	else
	{
		for (currentFitSlice_ = fitData_.first(); currentFitSlice_ != NULL; currentFitSlice_ = currentFitSlice_->next)
		{
			// Call the relevant minimiser
			simplexMinimise();
		}
	}
}

/*
 * Variables Group
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
	ui.VariablesTable->setRowCount(nVariablesUsed_);

	QTableWidgetItem* item;
	bool status, isXYZ;
	ReturnValue rv;
	int count = 0, n;
	for (int n=0; n<equationVariables_.nItems(); ++n)
	{
		// Grab the fit variable and see if it's used in the current equation?
		EquationVariable* eqVar = equationVariables_[n];
		if (!eqVar->used()) continue;

		// Grab variable pointer from FitVariable
		Variable* var = eqVar->variable();

		// -- Variable Name / Fit flag
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
		item->setCheckState(eqVar->fit() ? Qt::Checked : Qt::Unchecked);
		item->setText(var->name());
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 0, item);

		// -- Value
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		item->setText(QString::number(eqVar->value()));
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 1, item);

		// -- Minimum limit
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		item->setCheckState(eqVar->minimumLimitEnabled() ? Qt::Checked : Qt::Unchecked);
		item->setText(QString::number(eqVar->minimumLimit()));
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 2, item);

		// -- Maximum limit
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		item->setCheckState(eqVar->maximumLimitEnabled() ? Qt::Checked : Qt::Unchecked);
		item->setText(QString::number(eqVar->maximumLimit()));
		item->setData(Qt::UserRole, n);
		ui.VariablesTable->setItem(count, 3, item);
		
		++count;
	}

	refreshing_ = false;
}

void FitDialog::on_VariablesTable_cellChanged(int row, int column)
{
	if (refreshing_) return;

	// The index of the associated FirVariable will have been stored as the item's data
	QTableWidgetItem* item = ui.VariablesTable->item(row, column);
	if (!item) return;

	// Get associated data
	int index = item->data(Qt::UserRole).toInt();
	EquationVariable* eqVariable = equationVariables_[index];
	if (!eqVariable) return;

	// Our action depends on the column edited...
	switch (column)
	{
		// Variable name / fit flag
		case (0):
			eqVariable->setFit(item->checkState() == Qt::Checked);
			break;
		// Variable value
		case (1):
			eqVariable->setValue(item->text().toDouble());
			break;
		// Minimum value and limit flag
		case (2):
			eqVariable->setMinimumLimit(item->checkState() == Qt::Checked, item->text().toDouble());
			break;
		// Maximum value and limit flag
		case (3):
			eqVariable->setMaximumLimit(item->checkState() == Qt::Checked, item->text().toDouble());
			break;
	}
}

/*
 * Source Group
 */

// Update source data group
void FitDialog::updateSourceGroup(bool refreshList)
{
	refreshing_ = true;

	// Get currently selected collection (by index)
	if (refreshList)
	{
		int currentIndex = ui.SourceCollectionCombo->currentIndex();
		ui.SourceCollectionCombo->clear();
		for (Collection* c = uChroma_->collections(); c != NULL; c = c->next) ui.SourceCollectionCombo->addItem(c->title());
		if ((currentIndex != -1) && (currentIndex < ui.SourceCollectionCombo->count())) ui.SourceCollectionCombo->setCurrentIndex(currentIndex);
		else ui.SourceCollectionCombo->setCurrentIndex(0);
	}

	// Grab current collection
	Collection* collection = uChroma_->collection(ui.SourceCollectionCombo->currentIndex());
	if (!collection)
	{
		refreshing_ = false;
		return;
	}

	// Update spin boxes with limits
	ui.SourceXYSliceFromSpin->setRange(1, ui.SourceXYSliceToSpin->value());
	ui.SourceXYSliceToSpin->setRange(ui.SourceXYSliceFromSpin->value(), collection->nSlices());
	ui.SourceXYFromZLabel->setText("Z = " + QString::number(collection->slice(ui.SourceXYSliceFromSpin->value())->data().z()));
	ui.SourceXYToZLabel->setText("Z = " + QString::number(collection->slice(ui.SourceXYSliceToSpin->value())->data().z()));
	ui.SourceXYXMinSpin->setRange(true, collection->transformMin().x, true, collection->transformMax().x);
	ui.SourceXYXMaxSpin->setRange(true, collection->transformMin().x, true, collection->transformMax().x);
}

void FitDialog::on_SourceCollectionCombo_currentIndexChanged(int index)
{
}

void FitDialog::on_SourceXYSliceFromSpin_valueChanged(int value)
{
	ui.SourceXYSliceToSpin->setMinimum(value);
}

void FitDialog::on_SourceXYSliceToSpin_valueChanged(int value)
{
	ui.SourceXYSliceFromSpin->setMaximum(value);
}

/*
 * Destination Group
 */

// Update destination data group
void FitDialog::updateDestinationGroup()
{
	// Get currently selected collection (by user data value)
	QVariant data = ui.DestinationCollectionCombo->itemData(ui.DestinationCollectionCombo->currentIndex());
	int currentIndex = data.toInt();

	// Grab current collection
	Collection* collection = uChroma_->collection(ui.SourceCollectionCombo->currentIndex());
	if (!collection) return;

	ui.DestinationCollectionCombo->clear();
	int index = 0;
	for (Collection* c = uChroma_->collections(); c != NULL; c = c->next, ++index) if (c != collection)
	{
		ui.DestinationCollectionCombo->addItem(c->title(), QVariant(index));
		if (currentIndex == index) ui.DestinationCollectionCombo->setCurrentIndex(currentIndex);
	}
}
