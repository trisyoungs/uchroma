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
#include "gui/referencedialog.h"
#include "gui/uchroma.h"
#include "base/collection.h"
#include "expression//variable.h"
#include "kernels/fit.h"
#include "templates/variantpointer.h"
#include <QtGui/QMessageBox>

// Constructor
FitSetupDialog::FitSetupDialog(UChromaWindow& parent) : QDialog(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	fitKernelTarget_ = NULL;
	softReject_ = false;

	refreshing_ = true;

	// Add items to MethodCombo
	for (int n=0; n<FitKernel::nMinimisationMethods; ++n) ui.MinimisationMethodCombo->addItem(FitKernel::minimisationMethod( (FitKernel::MinimisationMethod) n));

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
	reject();
}

void FitSetupDialog::reject()
{
	if (!softReject_)
	{
		// Revert to stored data and nullify pointer
		(*fitKernelTarget_) = fitKernelBackup_;
		fitKernelTarget_ = NULL;
	}

	softReject_ = false;
	setResult(QDialog::Rejected);
	hide();
}

void FitSetupDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

void FitSetupDialog::on_OKButton_clicked(bool checked)
{
	fitKernelTarget_ = NULL;
	accept();
}

/*
 * Fitting Data Setup
 */

// Set new FitKernel target
bool FitSetupDialog::setFitKernel(FitKernel* fitKernel)
{
	// Check for presence of existing fitKernel...
	if (fitKernelTarget_)
	{
		QMessageBox::warning(this, "Fit Dialog Busy", "The fit setup dialog is already busy and waiting for input (perhaps waiting for a range to be selected?).");
		return false;
	}

	fitKernelTarget_ = fitKernel;
	fitKernelTarget_->checkRanges();

	// Store backup in case the dialog is 
	fitKernelBackup_ = (*fitKernelTarget_);
}

/*
 * Equations Group
 */

void FitSetupDialog::on_EquationEdit_textChanged(QString text)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->resetEquation();
	if (fitKernelTarget_->setEquation(text))
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

/*
 * Variables Group
 */

// Update variable table
void FitSetupDialog::updateVariableTable()
{
	refreshing_ = true;

	// Clear table, but don't display anything if equation is not valid
	ui.VariablesTable->clearContents();
	if (!fitKernelTarget_->equationValid())
	{
		refreshing_ = false;
		return;
	}
	ui.VariablesTable->setRowCount(fitKernelTarget_->nVariablesUsed());

	QTableWidgetItem* item;
	bool status, isXYZ;
	int count = 0;
	for (RefListItem<EquationVariable,bool>* ri = fitKernelTarget_->usedVariables(); ri != NULL; ri = ri->next)
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

	// The index of the associated FitVariable will have been stored as the item's data
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

void FitSetupDialog::updateReferencesTable()
{
	ui.VariableReferenceList->clear();
	for (ReferenceVariable* refVar = fitKernelTarget_->references(); refVar != NULL; refVar = refVar->next)
	{
		QListWidgetItem* item = new QListWidgetItem;
		QString s = refVar->name();
		if (refVar->xIndex().type() == IndexData::NormalIndex) s += " [X=norm,";
		else if (refVar->xIndex().type() == IndexData::FixedIndex) s += " [X=fixed("+QString::number(refVar->xIndex().index()+1)+"),";
		else if (refVar->xIndex().type() == IndexData::RelativeIndex) s += " [X=rel("+QString::number(refVar->xIndex().offset())+"),";
		if (refVar->zIndex().type() == IndexData::NormalIndex) s += "Z=norm]";
		else if (refVar->zIndex().type() == IndexData::FixedIndex) s += "Z=fixed("+QString::number(refVar->zIndex().index()+1)+")]";
		else if (refVar->zIndex().type() == IndexData::RelativeIndex) s += "Z=rel("+QString::number(refVar->zIndex().offset())+")]";
		item->setText(s);
		item->setData(Qt::UserRole, VariantPointer<ReferenceVariable>(refVar));
		ui.VariableReferenceList->addItem(item);
	}
}

void FitSetupDialog::on_VariableAddReferenceButton_clicked(bool checked)
{
	// Create a new reference variable
	ReferenceVariable* refVar = fitKernelTarget_->addReference(fitKernelTarget_->uniqueReferenceName("y"));

	// Create a ReferenceSetupDialog
	ReferenceSetupDialog referenceSetup(this);
	if (!referenceSetup.call(refVar, fitKernelTarget_)) fitKernelTarget_->removeReference(refVar);
	else
	{
		// This is a new reference variable, so let's see if the variable it targets already exists in the equation
		EquationVariable* var = fitKernelTarget_->variable(refVar->name());
		if (var)
		{
			// Update the equation (just try to set it again) and refresh controls
			fitKernelTarget_->setEquation(ui.EquationEdit->text());
		}
		updateControls();
	}
}

void FitSetupDialog::on_VariableReferenceList_itemDoubleClicked(QListWidgetItem* item)
{
	if (!item) return;
	ReferenceVariable* refVar = VariantPointer<ReferenceVariable>(item->data(Qt::UserRole));
	if (!refVar) return;

	// Create a ReferenceSetupDialog
	ReferenceSetupDialog referenceSetup(this);
	if (referenceSetup.call(refVar, fitKernelTarget_))
	{
		// If the name of the reference was changed we will have to flag the equation as invalid...
		if (refVar->updateVariable() && (refVar->used())) fitKernelTarget_->setEquationInvalid();

		updateControls();
	}
}

/*
 * Source Data
 */

void FitSetupDialog::on_DataNormalFitRadio_clicked(bool checked)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setOrthogonal(!checked);
}

void FitSetupDialog::on_DataOrthogonalFitRadio_clicked(bool checked)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setOrthogonal(checked);
}

void FitSetupDialog::on_DataGlobalFitCheck_clicked(bool checked)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setGlobal(checked);
}

/*
 * Source X
 */

void FitSetupDialog::on_XSourceAbsoluteRadio_toggled(bool checked)
{
	if (!checked) return;

	ui.XAbsoluteMinSpin->setEnabled(true);
	ui.XAbsoluteMaxSpin->setEnabled(true);
	ui.XPointSingleSpin->setEnabled(false);
	ui.XPointMaxSpin->setEnabled(false);
	ui.XPointMinSpin->setEnabled(false);
	ui.XPointSingleLabel->setEnabled(false);
	ui.XPointMaxLabel->setEnabled(false);
	ui.XPointMinLabel->setEnabled(false);

	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setXRange(FitKernel::AbsoluteRange);
}

void FitSetupDialog::on_XAbsoluteMinSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setAbsoluteXMin(value);

	updateLabels();
}

void FitSetupDialog::on_XAbsoluteMaxSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setAbsoluteXMax(value);

	updateLabels();
}

void FitSetupDialog::on_XAbsoluteSelectButton_clicked(bool checked)
{
	uChroma_.setInteractionMode(InteractionMode::FitSetupSelectXInteraction, 0);
	softReject_ = true;
	reject();
}

void FitSetupDialog::on_XSourceSinglePointRadio_toggled(bool checked)
{
	if (!checked) return;

	ui.XAbsoluteMinSpin->setEnabled(false);
	ui.XAbsoluteMaxSpin->setEnabled(false);
	ui.XPointSingleSpin->setEnabled(true);
	ui.XPointMaxSpin->setEnabled(false);
	ui.XPointMinSpin->setEnabled(false);
	ui.XPointSingleLabel->setEnabled(true);
	ui.XPointMaxLabel->setEnabled(false);
	ui.XPointMinLabel->setEnabled(false);

	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setXRange(FitKernel::SinglePointRange);
}

void FitSetupDialog::on_XPointSingleSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setIndexXSingle(value);

	updateLabels();
}

void FitSetupDialog::on_XSourcePointRangeRadio_toggled(bool checked)
{
	if (!checked) return;

	ui.XAbsoluteMinSpin->setEnabled(false);
	ui.XAbsoluteMaxSpin->setEnabled(false);
	ui.XPointSingleSpin->setEnabled(false);
	ui.XPointMaxSpin->setEnabled(true);
	ui.XPointMinSpin->setEnabled(true);
	ui.XPointSingleLabel->setEnabled(false);
	ui.XPointMaxLabel->setEnabled(true);
	ui.XPointMinLabel->setEnabled(true);

	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setXRange(FitKernel::IndexRange);
}

void FitSetupDialog::on_XPointMinSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setAbsoluteXMax(value);

	updateLabels();
}

void FitSetupDialog::on_XPointMaxSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setAbsoluteXMax(value);

	updateLabels();
}

/*
 * Source Z
 */

void FitSetupDialog::on_ZSourceAbsoluteRadio_toggled(bool checked)
{
	if (!checked) return;

	ui.ZAbsoluteMinSpin->setEnabled(true);
	ui.ZAbsoluteMaxSpin->setEnabled(true);
	ui.ZDataSetCombo->setEnabled(false);
	ui.ZDataSetMaxSpin->setEnabled(false);
	ui.ZDataSetMinSpin->setEnabled(false);
	ui.ZDataSetSingleLabel->setEnabled(false);
	ui.ZDataSetMaxLabel->setEnabled(false);
	ui.ZDataSetMinLabel->setEnabled(false);

	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setZRange(FitKernel::AbsoluteRange);
}

void FitSetupDialog::on_ZAbsoluteMinSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setAbsoluteZMin(value);

	updateLabels();
}

void FitSetupDialog::on_ZAbsoluteMaxSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setAbsoluteZMax(value);

	updateLabels();
}

void FitSetupDialog::on_ZAbsoluteSelectButton_clicked(bool checked)
{
	uChroma_.setInteractionMode(InteractionMode::FitSetupSelectZInteraction, 2);
	softReject_ = true;
	reject();
}

void FitSetupDialog::on_ZSourceSingleDataSetRadio_toggled(bool checked)
{
	if (!checked) return;

	ui.ZAbsoluteMinSpin->setEnabled(false);
	ui.ZAbsoluteMaxSpin->setEnabled(false);
	ui.ZDataSetCombo->setEnabled(true);
	ui.ZDataSetMaxSpin->setEnabled(false);
	ui.ZDataSetMinSpin->setEnabled(false);
	ui.ZDataSetSingleLabel->setEnabled(true);
	ui.ZDataSetMaxLabel->setEnabled(false);
	ui.ZDataSetMinLabel->setEnabled(false);

	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setZRange(FitKernel::SinglePointRange);
}

void FitSetupDialog::on_ZDataSetCombo_currentIndexChanged(int index)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setIndexZSingle(index);

	updateLabels();
}

void FitSetupDialog::on_ZSourceDataSetRangeRadio_toggled(bool checked)
{
	if (!checked) return;

	ui.ZAbsoluteMinSpin->setEnabled(false);
	ui.ZAbsoluteMaxSpin->setEnabled(false);
	ui.ZDataSetCombo->setEnabled(false);
	ui.ZDataSetMaxSpin->setEnabled(true);
	ui.ZDataSetMinSpin->setEnabled(true);
	ui.ZDataSetSingleLabel->setEnabled(false);
	ui.ZDataSetMaxLabel->setEnabled(true);
	ui.ZDataSetMinLabel->setEnabled(true);

	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setZRange(FitKernel::IndexRange);
}

void FitSetupDialog::on_ZDataSetMinSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setIndexZMin(value);

	updateLabels();
}

void FitSetupDialog::on_ZDataSetMaxSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setIndexZMax(value);

	updateLabels();
}

/*
 * Minimisation
 */

void FitSetupDialog::on_MinimisationMethodCombo_currentIndexChanged(int index)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setMethod((FitKernel::MinimisationMethod) index);
}

void FitSetupDialog::on_MinimisationToleranceSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setTolerance(value);
}

void FitSetupDialog::on_MinimisationMaxStepsSpin_valueChanged(int value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setMaxSteps(value);
}

void FitSetupDialog::on_MinimisationLimitStrengthSpin_valueChanged(double value)
{
	if (refreshing_ || (!fitKernelTarget_)) return;

	fitKernelTarget_->setLimitStrength(value);
}

/*
 * Update Functions
 */

// Update controls and execute dialog
int FitSetupDialog::updateAndExec()
{
	updateControls(true);
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
	return exec();
}

// Update controls
void FitSetupDialog::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if (!isVisible() && (!force)) return;

	Collection* sourceCollection = fitKernelTarget_->sourceCollection();
	if (!sourceCollection) return;

	refreshing_ = true;

	// Source Collection Group
	ui.SourceCollectionLabel->setText(sourceCollection->name());

	// Destination Collection Group
	if (fitKernelTarget_->destinationCollection()) ui.DestinationCollectionLabel->setText(fitKernelTarget_->destinationCollection()->name());
	else ui.DestinationCollectionLabel->setText("<None>");

	// Equation Group
	ui.EquationEdit->setText(fitKernelTarget_->equationText());

	// Source Data Group
	if (fitKernelTarget_->orthogonal()) ui.DataOrthogonalFitRadio->setChecked(true);
	else ui.DataNormalFitRadio->setChecked(true);
	ui.DataGlobalFitCheck->setChecked(fitKernelTarget_->global());

	// Source X
	if (fitKernelTarget_->xRange() == FitKernel::AbsoluteRange) ui.XSourceAbsoluteRadio->setChecked(true);
	else if (fitKernelTarget_->xRange() == FitKernel::SinglePointRange) ui.XSourceSinglePointRadio->setChecked(true);
	else ui.XSourcePointRangeRadio->setChecked(true);
	ui.XAbsoluteMinSpin->setValue(fitKernelTarget_->absoluteXMin());
	ui.XAbsoluteMinSpin->setRange(true, sourceCollection->transformMin().x, true, sourceCollection->transformMax().x);
	ui.XAbsoluteMaxSpin->setValue(fitKernelTarget_->absoluteXMax());
	ui.XAbsoluteMaxSpin->setRange(true, sourceCollection->transformMin().x, true, sourceCollection->transformMax().x);
	ui.XPointSingleSpin->setValue(fitKernelTarget_->indexXSingle()+1);
	ui.XPointSingleSpin->setRange(1, sourceCollection->displayAbscissa().nItems());
	ui.XPointMinSpin->setValue(fitKernelTarget_->indexXMin()+1);
	ui.XPointMinSpin->setRange(1, sourceCollection->displayAbscissa().nItems());
	ui.XPointMaxSpin->setValue(fitKernelTarget_->indexXMax()+1);
	ui.XPointMaxSpin->setRange(1, sourceCollection->displayAbscissa().nItems());

	// Source Z
	if (fitKernelTarget_->zRange() == FitKernel::AbsoluteRange) ui.ZSourceAbsoluteRadio->setChecked(true);
	else if (fitKernelTarget_->zRange() == FitKernel::SinglePointRange) ui.ZSourceSingleDataSetRadio->setChecked(true);
	else ui.ZSourceDataSetRangeRadio->setChecked(true);
	ui.ZAbsoluteMinSpin->setValue(fitKernelTarget_->absoluteZMin());
	ui.ZAbsoluteMinSpin->setRange(true, sourceCollection->transformMin().z, true, sourceCollection->transformMax().z);
	ui.ZAbsoluteMaxSpin->setValue(fitKernelTarget_->absoluteZMax());
	ui.ZAbsoluteMaxSpin->setRange(true, sourceCollection->transformMin().z, true, sourceCollection->transformMax().z);
	ui.ZDataSetCombo->clear();
	for (DataSet* dataSet = sourceCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next) ui.ZDataSetCombo->addItem(dataSet->name());
	ui.ZDataSetCombo->setCurrentIndex(fitKernelTarget_->indexZSingle());
	ui.ZDataSetMinSpin->setValue(fitKernelTarget_->indexZMin()+1);
	ui.ZDataSetMinSpin->setRange(1, sourceCollection->nDataSets());
	ui.ZDataSetMaxSpin->setValue(fitKernelTarget_->indexZMax()+1);
	ui.ZDataSetMaxSpin->setRange(1, sourceCollection->nDataSets());

	// Minimisation Group
	ui.MinimisationMethodCombo->setCurrentIndex(fitKernelTarget_->method());
	ui.MinimisationToleranceSpin->setValue(fitKernelTarget_->tolerance());
	ui.MinimisationMaxStepsSpin->setValue(fitKernelTarget_->maxSteps());
	ui.MinimisationLimitStrengthSpin->setValue(fitKernelTarget_->limitStrength());

	refreshing_ = false;

	// Update variables and references tables
	updateVariableTable();
	updateReferencesTable();

	// Update labels
	updateLabels();
}

// Update labels
void FitSetupDialog::updateLabels()
{
	if (refreshing_) return;

	Collection* sourceCollection = fitKernelTarget_->sourceCollection();
	if (!sourceCollection) return;

	// X Source
	const Array<double>& abscissa = sourceCollection->displayAbscissa();
	ui.XPointSingleLabel->setText("(X = " + QString::number(abscissa.value(ui.XPointSingleSpin->value())) + ")");
	ui.XPointMinLabel->setText("(X = " + QString::number(abscissa.value(ui.XPointMinSpin->value())) + ")");
	ui.XPointMaxLabel->setText("(X = " + QString::number(abscissa.value(ui.XPointMaxSpin->value())) + ")");

	// Z Source
	DataSet* dataSet = sourceCollection->nDataSets() == 0 ? NULL : sourceCollection->dataSet(ui.ZDataSetCombo->currentIndex());
	ui.ZDataSetSingleLabel->setText("(Z = " + (dataSet ? QString::number(dataSet->data().z()) + ")" : "?)"));
	dataSet = sourceCollection->nDataSets() == 0 ? NULL : sourceCollection->dataSet(ui.ZDataSetMinSpin->value()-1);
	ui.ZDataSetMinLabel->setText("(Z = " + (dataSet ? QString::number(dataSet->data().z()) + ")" : "?)"));
	dataSet = sourceCollection->nDataSets() == 0 ? NULL : sourceCollection->dataSet(ui.ZDataSetMaxSpin->value()-1);
	ui.ZDataSetMaxLabel->setText("(Z = " + (dataSet ? QString::number(dataSet->data().z()) + ")" : "?)"));
}
