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
#include "gui/equationselect.h"
#include "templates/variantpointer.h"

// Constructor
FitWindow::FitWindow(UChromaWindow& parent) : QWidget(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	// Set default values in some widgets
	ui.MethodSDToleranceSpin->setValue(1.0e-4);
	ui.OptionsLimitStrengthSpin->setValue(10000.0);

	refreshing_ = false;
	sourceCollection_ = NULL;
	resetEquation();
	ui.EquationEdit->setText("cos(x)*A");
	equationValid_ = equation_.setCommands("cos(x)*A");
	updateVariables();
}

// Destructor
FitWindow::~FitWindow()
{
}

/*
 * Window Functions
 */

// Send message to message box
void FitWindow::printMessage(const char* fmtString, ...) const
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
void FitWindow::updateMainView()
{
	if (ui.OptionsVisualCheck->isChecked())
	{
		updateFittedData();
		uChroma_.updateDisplay();
	}
}

// Window close event
void FitWindow::closeEvent(QCloseEvent* event)
{
	emit(windowClosed(false));
}

/*
 * Equations Group
 */

void FitWindow::on_EquationEdit_textChanged(QString text)
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

void FitWindow::on_SelectEquationButton_clicked(bool checked)
{
	EquationSelectDialog selectDialog(this);
	if (selectDialog.exec()) ui.EquationEdit->setText(selectDialog.selectedEquation().equationText);
}

void FitWindow::on_FitButton_clicked(bool checked)
{
	if (doFitting()) uChroma_.setAsModified();
	uChroma_.updateGUI();
}

/*
 * Variables Group
 */

// Update variable table
void FitWindow::updateVariableTable()
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

void FitWindow::on_VariablesTable_cellChanged(int row, int column)
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

// Update source data
void FitWindow::updateSourceData(bool setInitialValues)
{
	if (!sourceCollection_) return;

	refreshing_ = true;

	// Set collection name
	ui.SourceCollectionLabel->setText(sourceCollection_->title());
	// Populate reference Y combo
	ui.SourceReferenceYCombo->clear();
	for (DataSet* dataSet = sourceCollection_->dataSets(); dataSet != NULL; dataSet = dataSet->next) ui.SourceReferenceYCombo->addItem(dataSet->title() + " (Z = " + QString::number(dataSet->data().z()) + ")");

	// Update spin boxes with limits
	ui.SourceDataSetFromSpin->setRange(1, ui.SourceDataSetToSpin->value());
	ui.SourceDataSetToSpin->setRange(ui.SourceDataSetFromSpin->value(), sourceCollection_->nDataSets());
	ui.SourceXMinSpin->setRange(true, sourceCollection_->transformMin().x, true, sourceCollection_->transformMax().x);
	ui.SourceXMaxSpin->setRange(true, sourceCollection_->transformMin().x, true, sourceCollection_->transformMax().x);

	// Set initial values for x and z ranges if requested
	if (setInitialValues)
	{
		ui.SourceDataSetFromSpin->setValue(1);
		ui.SourceDataSetToSpin->setValue(sourceCollection_->nDataSets());
		ui.SourceXMinSpin->setValue(sourceCollection_->transformMin().x);
		ui.SourceXMaxSpin->setValue(sourceCollection_->transformMax().x);
	}

	// Update info labels
	DataSet* dataSet = sourceCollection_->nDataSets() == 0 ? NULL : sourceCollection_->dataSet(ui.SourceDataSetFromSpin->value()-1);
	ui.SourceFromZLabel->setText("Z = " + (dataSet ? QString::number(dataSet->data().z()) : "???"));
	dataSet = sourceCollection_->nDataSets() == 0 ? NULL : sourceCollection_->dataSet(ui.SourceDataSetToSpin->value()-1);
	ui.SourceToZLabel->setText("Z = " + (dataSet ? QString::number(dataSet->data().z()) : "???"));

	refreshing_ = false;
}

void FitWindow::on_SourceDataSetFromSpin_valueChanged(int value)
{
	if (refreshing_) return;
	updateSourceData(false);
}

void FitWindow::on_SourceDataSetToSpin_valueChanged(int value)
{
	if (refreshing_) return;
	updateSourceData(false);
}

void FitWindow::on_SourceXSelectButton_clicked(bool checked)
{
	uChroma_.setInteractionMode(UChromaWindow::FitDialogSelectXInteraction, 0);
	hide();
}

/*
 * Destination Group
 */

// Update destination data group
void FitWindow::updateDestinationGroup()
{
	if (!sourceCollection_) return;

	refreshing_ = true;

	ui.DestinationCollectionCombo->clear();
	for (Collection* c = sourceCollection_->fitData(); c != NULL; c = c->next)
	{
		ui.DestinationCollectionCombo->addItem(c->title(), VariantPointer<Collection>(c));
	}

	refreshing_ = false;
}

/*
 * Update Functions
 */

// Update controls and show window
void FitWindow::updateAndShow()
{
	show();
	updateControls();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void FitWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	refreshing_ = true;

	// Check currentCollection_ in uChroma_ - if it's the same as it was before, don't update any initial values
	bool updateInitialValues = uChroma_.currentCollection() != sourceCollection_;
	sourceCollection_ = uChroma_.currentCollection();
	updateSourceData(updateInitialValues);

	updateDestinationGroup();
	
	refreshing_ = false;
}
