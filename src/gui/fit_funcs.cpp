/*
	*** Fit Widget Functions
	*** src/gui/fit_funcs.cpp
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

#include "gui/fit.h"
#include "gui/uchroma.h"
#include "templates/variantpointer.h"

// Static singletons
UChromaWindow* FitDialog::uChroma_ = NULL;

// Constructor
FitDialog::FitDialog(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);

	// Set default values in some widgets
	ui.MethodSDToleranceSpin->setValue(1.0e-2);

	refreshing_ = false;
	resetEquation();
	ui.EquationEdit->setText("cos(x)*A");
	equationValid_ = equation_.setCommands("cos(x)*A");
	updateVariables();
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
 * Window Functions
 */

// Send message to message box
void FitDialog::printMessage(const char* fmtString, ...) const
{
	va_list arguments;
	static char msgs[8096];
	msgs[0] = '\0';
	// Parse the argument list (...) and internally write the output string into msgs[]
	va_start(arguments, fmtString);
	vsprintf(msgs, fmtString, arguments);
	ui.OutputEdit->append(msgs);
	va_end(arguments);
}

// Refresh main view (if options permit) after fit params have changed
void FitDialog::updateMainView()
{
	if (ui.OptionsVisualCheck->isChecked())
	{
		updateFittedData();
		uChroma_->updateDisplay();
	}
}

// Update data in window
void FitDialog::updateAll()
{
	updateSourceGroup();
	updateDestinationGroup();
}

/*
 * Widgets / Slots / Reimplementations
 */

void FitDialog::on_CloseButton_clicked(bool checked)
{
	uChroma_->updateGUI();

	hide();
}

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
		ui.FitButton->setEnabled(true);
	}
	else
	{
		QPalette redText;
		redText.setColor(QPalette::Text, Qt::red);
		ui.EquationEdit->setPalette(redText);
		ui.FitButton->setEnabled(false);
	}

	// Update list of variables
	updateVariables();
	updateVariableTable();
}

void FitDialog::on_SelectEquationButton_clicked(bool checked)
{
}

void FitDialog::on_FitButton_clicked(bool checked)
{
	if (doFitting()) uChroma_->setAsModified();
	uChroma_->updateGUI();
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

	// Update collection list
	if (refreshList)
	{
		Collection* currentCollection = VariantPointer<Collection>(ui.SourceCollectionCombo->itemData(ui.SourceCollectionCombo->currentIndex()));
		ui.SourceCollectionCombo->clear();
		int index = 0;
		for (Collection* c = uChroma_->collections(); c != NULL; c = c->next, ++index)
		{
			ui.SourceCollectionCombo->addItem(c->title());
			if (c == currentCollection) ui.SourceCollectionCombo->setCurrentIndex(index);
		}
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
	ui.SourceXYXMinSpin->setRange(true, collection->transformMin().x, true, collection->transformMax().x);
	ui.SourceXYXMaxSpin->setRange(true, collection->transformMin().x, true, collection->transformMax().x);

	// Set initial values for x range if refreshing list
	if (refreshList)
	{
		ui.SourceXYSliceFromSpin->setValue(1);
		ui.SourceXYSliceToSpin->setValue(collection->nSlices());
		ui.SourceXYXMinSpin->setValue(collection->transformMin().x);
		ui.SourceXYXMaxSpin->setValue(collection->transformMax().x);
	}

	// Update info labels
	Slice* slice = collection->slice(ui.SourceXYSliceFromSpin->value()-1);
	ui.SourceXYFromZLabel->setText("Z = " + (slice ? QString::number(slice->data().z()) : "???"));
	slice = collection->slice(ui.SourceXYSliceToSpin->value()-1);
	ui.SourceXYToZLabel->setText("Z = " + (slice ? QString::number(slice->data().z()) : "???"));

	refreshing_ = false;
}

void FitDialog::on_SourceCollectionCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	updateSourceGroup();
}

void FitDialog::on_SourceXYSliceFromSpin_valueChanged(int value)
{
	if (refreshing_) return;
	updateSourceGroup(false);
}

void FitDialog::on_SourceXYSliceToSpin_valueChanged(int value)
{
	if (refreshing_) return;
	updateSourceGroup(false);
}

/*
 * Destination Group
 */

// Update destination data group
void FitDialog::updateDestinationGroup()
{
	refreshing_ = true;

	// Get currently selected collection (by user data value)
	Collection* selectedCollection = VariantPointer<Collection>(ui.DestinationCollectionCombo->itemData(ui.DestinationCollectionCombo->currentIndex()));

	// Grab current collection
	Collection* collection = uChroma_->collection(ui.SourceCollectionCombo->currentIndex());
	if (!collection) return;

	ui.DestinationCollectionCombo->clear();
	int index = 0;
	for (Collection* c = uChroma_->collections(); c != NULL; c = c->next, ++index) if (c != collection)
	{
		ui.DestinationCollectionCombo->addItem(c->title(), VariantPointer<Collection>(c));
		if (selectedCollection == c) ui.DestinationCollectionCombo->setCurrentIndex(index);
	}

	refreshing_ = false;
}
