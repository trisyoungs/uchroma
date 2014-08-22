/*
	*** FitSetup Dialog Functions
	*** src/gui/fitsetup_funcs.cpp
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

#include "gui/fitsetup.h"
#include "gui/equationselect.h"
#include "gui/uchroma.h"
#include "base/collection.h"
#include "kernels/fit.h"
#include "templates/variantpointer.h"
#include <QtGui/QMessageBox>

// Constructor
FitSetupDialog::FitSetupDialog(UChromaWindow& parent) : QDialog(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	fitKernel_ = NULL;

	refreshing_ = false;
}

// Destructor
FitSetupDialog::~FitSetupDialog()
{
}

/*
 * Window Functions
 */

// Window close event
void FitSetupDialog::closeEvent(QCloseEvent* event)
{
	fitKernel_ = NULL;
	reject();
}

void FitSetupDialog::on_CloseButton_clicked(bool checked)
{
	accept();
}

/*
 * Fitting Data Setup
 */

// Set new FitKernel target
bool FitSetupDialog::setFitKernel(FitKernel* fitKernel)
{
	// Check for presence of existing fitKernel...
	if (fitKernel_)
	{
		QMessageBox::warning(this, "Fit Dialog Busy", "The fit setup dialog is already busy and waiting for input (perhaps waiting for a range to be selected?).");
		return false;
	}

	fitKernel_ = fitKernel;
}

/*
 * Equations Group
 */

void FitSetupDialog::on_EquationEdit_textChanged(QString text)
{
	fitKernel_->resetEquation();
	if (fitKernel_->setEquation(text))
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
	updateVariableTable();
}

void FitSetupDialog::on_SelectEquationButton_clicked(bool checked)
{
	EquationSelectDialog selectDialog(this);
	if (selectDialog.exec()) ui.EquationEdit->setText(selectDialog.selectedEquation().equationText);
}

// void FitSetupDialog::on_FitButton_clicked(bool checked)
// {
// 	if (doFitting()) uChroma_.setAsModified();
// 	uChroma_.updateGUI();
// }

/*
 * Variables Group
 */

// Update variable table
void FitSetupDialog::updateVariableTable()
{
	refreshing_ = true;

	// Clear table, but don't display anything if equation is not valid
	ui.VariablesTable->clearContents();
	if (!fitKernel_->equationValid())
	{
		refreshing_ = false;
		return;
	}
	ui.VariablesTable->setRowCount(fitKernel_->nVariablesUsed());

	QTableWidgetItem* item;
	bool status, isXYZ;
	ReturnValue rv;
	int count = 0;
	for (RefListItem<EquationVariable,bool>* ri = fitKernel_->fitVariables(); ri != NULL; ri = ri->next)
	{
		// Grab the fit variable and see if it's used in the current equation?
		EquationVariable* eqVar = ri->item;
		if (!eqVar->used()) continue;

		// Grab variable pointer from FitVariable
		Variable* var = eqVar->variable();

		// -- Variable Name / Fit flag
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
		item->setCheckState(eqVar->fit() ? Qt::Checked : Qt::Unchecked);
		item->setText(var->name());
		item->setData(Qt::UserRole, VariantPointer<EquationVariable>(eqVar));
		ui.VariablesTable->setItem(count, 0, item);

		// -- Value
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		item->setText(QString::number(eqVar->value()));
		item->setData(Qt::UserRole, VariantPointer<EquationVariable>(eqVar));
		ui.VariablesTable->setItem(count, 1, item);

		// -- Minimum limit
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		item->setCheckState(eqVar->minimumLimitEnabled() ? Qt::Checked : Qt::Unchecked);
		item->setText(QString::number(eqVar->minimumLimit()));
		item->setData(Qt::UserRole, VariantPointer<EquationVariable>(eqVar));
		ui.VariablesTable->setItem(count, 2, item);

		// -- Maximum limit
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		item->setCheckState(eqVar->maximumLimitEnabled() ? Qt::Checked : Qt::Unchecked);
		item->setText(QString::number(eqVar->maximumLimit()));
		item->setData(Qt::UserRole, VariantPointer<EquationVariable>(eqVar));
		ui.VariablesTable->setItem(count, 3, item);
		
		++count;
	}

	refreshing_ = false;
}

void FitSetupDialog::on_VariablesTable_cellChanged(int row, int column)
{
	if (refreshing_) return;

	// The index of the associated FirVariable will have been stored as the item's data
	QTableWidgetItem* item = ui.VariablesTable->item(row, column);
	if (!item) return;

	// Get associated data
	EquationVariable* eqVariable = VariantPointer<EquationVariable>(item->data(Qt::UserRole));
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

void FitSetupDialog::on_VariablesLimitStrengthSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setLimitStrength(value);
}

/*
 * Source Data
 */
void FitSetupDialog::on_DataNormalFitRadio_clicked(bool checked)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setOrthogonal(!checked);
}

void FitSetupDialog::on_DataOrthogonalFitRadio_clicked(bool checked)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setOrthogonal(checked);
}

void FitSetupDialog::on_DataGlobalFit_clicked(bool checked)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setGlobal(checked);
}

/*
 * Minimisation
 */
void FitSetupDialog::on_MinimisationMethodCombo_currentIndexChanged(int index)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setMethod((FitKernel::MinimisationMethod) index);
}

void FitSetupDialog::on_MinimisationToleranceSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setTolerance(value);
}

void FitSetupDialog::on_MinimisationMaxStepsSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setMaxSteps(value);
}

/*
 * Source X
 */

void FitSetupDialog::on_XSourceAbsoluteRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.XAbsoluteMinSpin->setEnabled(true);
	ui.XAbsoluteMaxSpin->setEnabled(true);
	ui.XPointSingleSpin->setEnabled(false);
	ui.XPointMaxSpin->setEnabled(false);
	ui.XPointMinSpin->setEnabled(false);

	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setXRange(FitKernel::AbsoluteRange);
}

void FitSetupDialog::on_XAbsoluteMinSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setAbsoluteXMin(value);
}

void FitSetupDialog::on_XAbsoluteMaxSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setAbsoluteXMax(value);
}

void FitSetupDialog::on_XAbsoluteSelectButton_clicked(bool checked)
{
	uChroma_.setInteractionMode(InteractionMode::FitSetupSelectXInteraction, 0);
	hide();
}

void FitSetupDialog::on_XSourceSinglePointRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.XAbsoluteMinSpin->setEnabled(false);
	ui.XAbsoluteMaxSpin->setEnabled(false);
	ui.XPointSingleSpin->setEnabled(true);
	ui.XPointMaxSpin->setEnabled(false);
	ui.XPointMinSpin->setEnabled(false);

	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setXRange(FitKernel::SinglePointRange);
}

void FitSetupDialog::on_XPointSingleSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setIndexXSingle(value);
}

void FitSetupDialog::on_XSourcePointRangeRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.XAbsoluteMinSpin->setEnabled(false);
	ui.XAbsoluteMaxSpin->setEnabled(false);
	ui.XPointSingleSpin->setEnabled(false);
	ui.XPointMaxSpin->setEnabled(true);
	ui.XPointMinSpin->setEnabled(true);

	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setXRange(FitKernel::IndexRange);
}

void FitSetupDialog::on_XPointMinSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setAbsoluteXMax(value);
}

void FitSetupDialog::on_XPointMaxSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setAbsoluteXMax(value);
}

/*
 * Source Z
 */
void FitSetupDialog::on_ZSourceAbsoluteRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.ZAbsoluteMinSpin->setEnabled(true);
	ui.ZAbsoluteMaxSpin->setEnabled(true);
	ui.ZDataSetCombo->setEnabled(false);
	ui.ZDataSetMaxSpin->setEnabled(false);
	ui.ZDataSetMinSpin->setEnabled(false);

	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setZRange(FitKernel::AbsoluteRange);
}

void FitSetupDialog::on_ZAbsoluteMinSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setAbsoluteZMin(value);
}

void FitSetupDialog::on_ZAbsoluteMaxSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setAbsoluteZMax(value);
}

void FitSetupDialog::on_ZAbsoluteSelectButton_clicked(bool checked)
{
	uChroma_.setInteractionMode(InteractionMode::FitSetupSelectZInteraction, 2);
	hide();
}

void FitSetupDialog::on_ZSourceSingleDataSetRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.ZAbsoluteMinSpin->setEnabled(false);
	ui.ZAbsoluteMaxSpin->setEnabled(false);
	ui.ZDataSetCombo->setEnabled(true);
	ui.ZDataSetMaxSpin->setEnabled(false);
	ui.ZDataSetMinSpin->setEnabled(false);

	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setZRange(FitKernel::SinglePointRange);
}

void FitSetupDialog::on_ZDataSetCombo_currentIndexChanged(int index)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setIndexZSingle(index);
}

void FitSetupDialog::on_ZSourceDataSetRangeRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.ZAbsoluteMinSpin->setEnabled(false);
	ui.ZAbsoluteMaxSpin->setEnabled(false);
	ui.ZDataSetCombo->setEnabled(false);
	ui.ZDataSetMaxSpin->setEnabled(true);
	ui.ZDataSetMinSpin->setEnabled(true);

	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setZRange(FitKernel::IndexRange);
}

void FitSetupDialog::on_ZDataSetMinSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setIndexZMin(value);
}

void FitSetupDialog::on_ZDataSetMaxSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernel_)) return;

	fitKernel_->setIndexZMax(value);
}

/*
 * Update Functions
 */

// Update controls and show window
void FitSetupDialog::updateAndShow()
{
	updateControls();
	show();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void FitSetupDialog::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if (!isVisible()) return;

	Collection* sourceCollection = fitKernel_->sourceCollection();
	if (!sourceCollection) return;

	refreshing_ = true;

	// Source Collection Group
	ui.SourceCollectionLabel->setText(sourceCollection->title());

	// Equation Group
	ui.EquationEdit->setText(fitKernel_->equationText());

	// Variables Group
	ui.VariablesLimitStrengthSpin->setValue(fitKernel_->limitStrength());
	
	// Source Data Group
	if (fitKernel_->orthogonal()) ui.DataOrthogonalFitRadio->setChecked(true);
	else ui.DataNormalFitRadio->setChecked(true);
	ui.DataGlobalFitCheck->setChecked(fitKernel_->global());

	// Minimisation Group
	ui.MinimisationMethodCombo->setCurrentIndex(fitKernel_->method());
	ui.MinimisationToleranceSpin->setValue(fitKernel_->tolerance());
	ui.MinimisationMaxStepsSpin->setValue(fitKernel_->maxSteps());

	// Source X
	if (fitKernel_->xRange() == FitKernel::AbsoluteRange) ui.XSourceAbsoluteRadio->setChecked(true);
	else if (fitKernel_->xRange() == FitKernel::SinglePointRange) ui.XSourceSinglePointRadio->setChecked(true);
	else ui.XSourcePointRangeRadio->setChecked(true);
	ui.XAbsoluteMinSpin->setValue(fitKernel_->absoluteXMin());
	ui.XAbsoluteMinSpin->setRange(true, sourceCollection->transformMin().x, true, sourceCollection->transformMax().x);
	ui.XAbsoluteMaxSpin->setValue(fitKernel_->absoluteXMax());
	ui.XAbsoluteMaxSpin->setRange(true, sourceCollection->transformMin().x, true, sourceCollection->transformMax().x);
	ui.XPointSingleSpin->setValue(fitKernel_->indexXSingle()+1);
	ui.XPointSingleSpin->setRange(1, sourceCollection->displayAbscissa().nItems());
	ui.XPointMinSpin->setValue(fitKernel_->indexXMin()+1);
	ui.XPointMinSpin->setRange(1, sourceCollection->displayAbscissa().nItems());
	ui.XPointMaxSpin->setValue(fitKernel_->indexXMax()+1);
	ui.XPointMaxSpin->setRange(1, sourceCollection->displayAbscissa().nItems());

	// Source Z
	if (fitKernel_->zRange() == FitKernel::AbsoluteRange) ui.ZSourceAbsoluteRadio->setChecked(true);
	else if (fitKernel_->zRange() == FitKernel::SinglePointRange) ui.ZSourceSingleDataSetRadio->setChecked(true);
	else ui.ZSourceDataSetRangeRadio->setChecked(true);
	ui.ZAbsoluteMinSpin->setValue(fitKernel_->absoluteZMin());
	ui.ZAbsoluteMinSpin->setRange(true, sourceCollection->transformMin().z, true, sourceCollection->transformMax().z);
	ui.ZAbsoluteMaxSpin->setValue(fitKernel_->absoluteZMax());
	ui.ZAbsoluteMaxSpin->setRange(true, sourceCollection->transformMin().z, true, sourceCollection->transformMax().z);
	ui.ZDataSetCombo->clear();
	for (DataSet* dataSet = sourceCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next) ui.ZDataSetCombo->addItem(dataSet->title());
	ui.ZDataSetCombo->setCurrentIndex(fitKernel_->indexZSingle()+1);
	ui.ZDataSetMinSpin->setValue(fitKernel_->indexZMin()+1);
	ui.ZDataSetMinSpin->setRange(1, sourceCollection->nDataSets());
	ui.ZDataSetMaxSpin->setValue(fitKernel_->indexZMax()+1);
	ui.ZDataSetMaxSpin->setRange(1, sourceCollection->nDataSets());

	// Populate reference Y combo  TODO??
// 	ui.SourceReferenceYCombo->clear();
// 	for (DataSet* dataSet = sourceCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next) ui.SourceReferenceYCombo->addItem(dataSet->title() + " (Z = " + QString::number(dataSet->data().z()) + ")");

	refreshing_ = false;

	// Update variables table
	updateVariableTable();

	// Update labels
	updateLabels();
}

// Update labels
void FitSetupDialog::updateLabels()
{
	Collection* sourceCollection = fitKernel_->sourceCollection();
	if (!sourceCollection) return;

	// X Source
	const Array<double>& abscissa = sourceCollection->displayAbscissa();
	ui.XPointSingleLabel->setText("(X = " + QString::number(abscissa.value(ui.XPointSingleSpin->value())) + ")");
	ui.XPointMinLabel->setText("(X = " + QString::number(abscissa.value(ui.XPointMinSpin->value())) + ")");
	ui.XPointMaxLabel->setText("(X = " + QString::number(abscissa.value(ui.XPointMaxSpin->value())) + ")");

	// Z Source
	DataSet* dataSet = sourceCollection->nDataSets() == 0 ? NULL : sourceCollection->dataSet(ui.ZDataSetCombo->currentIndex()-1);
	ui.ZDataSetSingleLabel->setText("(Z = " + (dataSet ? QString::number(dataSet->data().z()) + ")" : "???)"));
	dataSet = sourceCollection->nDataSets() == 0 ? NULL : sourceCollection->dataSet(ui.ZDataSetMinSpin->value()-1);
	ui.ZDataSetMinLabel->setText("(Z = " + (dataSet ? QString::number(dataSet->data().z()) + ")" : "???)"));
	dataSet = sourceCollection->nDataSets() == 0 ? NULL : sourceCollection->dataSet(ui.ZDataSetMaxSpin->value()-1);
	ui.ZDataSetMaxLabel->setText("(Z = " + (dataSet ? QString::number(dataSet->data().z()) + ")" : "???)"));
}
