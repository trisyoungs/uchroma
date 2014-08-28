/*
	*** Reference Setup Dialog
	*** src/gui/referencedialog_funcs.cpp
	Copyright T. Youngs 2013-2014

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

#include "gui/referencedialog.h"
#include "gui/uchroma.h"
#include "kernels/fit.h"

// Constructor
ReferenceSetupDialog::ReferenceSetupDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

// Destructor
ReferenceSetupDialog::~ReferenceSetupDialog()
{
}

/*
 * Window Functions
 */

// Window close event
void ReferenceSetupDialog::closeEvent(QCloseEvent* event)
{
	reject();
}

void ReferenceSetupDialog::reject()
{
	// Revert to saved data
	(*referenceTarget_) = referenceBackup_;

	setResult(QDialog::Rejected);
	hide();
}

void ReferenceSetupDialog::on_OKButton_clicked(bool checked)
{
	accept();
}

void ReferenceSetupDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

// Call dialog to edit specified reference
bool ReferenceSetupDialog::call(ReferenceVariable* target, FitKernel* parentKernel)
{
	// Check supplied var
	if ((!target) || (!parentKernel))
	{
		msg.print("Internal Error: NULL ReferenceVariable or FitKernel given to ReferenceSetupDialog::call().\n");
		return false;
	}
	if (!target->sourceCollection())
	{
		msg.print("Internal Error: No SourceCollection specified in ReferenceVariable.\n");
		return false;
	}

	// Take copy of supplied data
	referenceParent_ = parentKernel;
	referenceTarget_ = target;
	referenceBackup_ = (*referenceTarget_);

	refreshing_ = true;
	
	// Update widgets
	// -- Names
	ui.NameEdit->setText(referenceTarget_->name());
	ui.SourceCollectionLabel->setText(referenceTarget_->sourceCollection()->title());
	// -- X range
	if (referenceTarget_->xType() == ReferenceVariable::NormalReference) ui.XNormalRadio->setChecked(true);
	else if (referenceTarget_->xType() == ReferenceVariable::FixedReference) ui.XFixedRadio->setChecked(true);
	else if (referenceTarget_->xType() == ReferenceVariable::RelativeReference) ui.XRelativeRadio->setChecked(true);
	int nX = referenceTarget_->sourceCollection()->displayAbscissa().nItems();
	ui.XFixedSpin->setValue(referenceTarget_->xIndex()+1);
	ui.XFixedSpin->setRange(1, nX);
	ui.XRelativeSpin->setValue(referenceTarget_->xIndex());
	ui.XRelativeSpin->setRange(-nX, nX);
	// -- X range
	if (referenceTarget_->zType() == ReferenceVariable::NormalReference) ui.ZNormalRadio->setChecked(true);
	else if (referenceTarget_->zType() == ReferenceVariable::FixedReference) ui.ZFixedRadio->setChecked(true);
	else if (referenceTarget_->zType() == ReferenceVariable::RelativeReference) ui.ZRelativeRadio->setChecked(true);
	int nZ = referenceTarget_->sourceCollection()->nDataSets();
	ui.ZDataSetCombo->clear();
	for (DataSet* dataSet = referenceTarget_->sourceCollection()->dataSets(); dataSet != NULL; dataSet = dataSet->next) ui.ZDataSetCombo->addItem(dataSet->title());
	ui.ZDataSetCombo->setCurrentIndex(referenceTarget_->zIndex());
	ui.ZRelativeSpin->setValue(referenceTarget_->zIndex());
	ui.ZRelativeSpin->setRange(-nZ, nZ);

	refreshing_ = false;

	// Update labels
	updateLabels();

	// Call the dialog
	if (exec()) return true;
	else return false;
}

/*
 * Slots
 */

void ReferenceSetupDialog::on_NameEdit_textChanged(QString text)
{
	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setName(text);

	// Must check name against other references to prevent duplicates...
	ReferenceVariable* refVar = referenceParent_->reference(text);
	if ((!refVar) || (refVar == referenceTarget_) && (!text.isEmpty()))
	{
		ui.OKButton->setEnabled(true);
		ui.NameEdit->setPalette(ui.SourceCollectionLabel->palette());
	}
	else
	{
		ui.OKButton->setEnabled(false);
		QPalette redText;
		redText.setColor(QPalette::Text, Qt::red);
		ui.NameEdit->setPalette(redText);
	}
}

void ReferenceSetupDialog::on_XNormalRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.XFixedSpin->setEnabled(false);
	ui.XFixedLabel->setEnabled(false);
	ui.XFixedSpinLabel->setEnabled(false);
	ui.XRelativeSpin->setEnabled(false);
	ui.XRelativeSpinLabel->setEnabled(false);

	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setXType(ReferenceVariable::NormalReference);
}

void ReferenceSetupDialog::on_XFixedRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.XFixedSpin->setEnabled(true);
	ui.XFixedLabel->setEnabled(true);
	ui.XFixedSpinLabel->setEnabled(true);
	ui.XRelativeSpin->setEnabled(false);
	ui.XRelativeSpinLabel->setEnabled(false);

	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setXType(ReferenceVariable::FixedReference);
}

void ReferenceSetupDialog::on_XRelativeRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.XFixedSpin->setEnabled(false);
	ui.XFixedLabel->setEnabled(false);
	ui.XFixedSpinLabel->setEnabled(false);
	ui.XRelativeSpin->setEnabled(true);
	ui.XRelativeSpinLabel->setEnabled(true);

	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setXType(ReferenceVariable::RelativeReference);
}

void ReferenceSetupDialog::on_XFixedSpin_valueChanged(int value)
{
	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setXIndex(value-1);

	updateLabels();
}

void ReferenceSetupDialog::on_XRelativeSpin_valueChanged(int value)
{
	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setXOffset(value-1);

	updateLabels();
}

void ReferenceSetupDialog::on_ZNormalRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.ZDataSetCombo->setEnabled(false);
	ui.ZFixedLabel->setEnabled(false);
	ui.ZDataSetComboLabel->setEnabled(false);
	ui.ZRelativeSpin->setEnabled(false);
	ui.ZRelativeSpinLabel->setEnabled(false);

	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setZType(ReferenceVariable::NormalReference);
}

void ReferenceSetupDialog::on_ZFixedRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.ZDataSetCombo->setEnabled(true);
	ui.ZFixedLabel->setEnabled(true);
	ui.ZDataSetComboLabel->setEnabled(true);
	ui.ZRelativeSpin->setEnabled(false);
	ui.ZRelativeSpinLabel->setEnabled(false);

	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setZType(ReferenceVariable::FixedReference);
}

void ReferenceSetupDialog::on_ZRelativeRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.ZDataSetCombo->setEnabled(false);
	ui.ZFixedLabel->setEnabled(false);
	ui.ZDataSetComboLabel->setEnabled(false);
	ui.ZRelativeSpin->setEnabled(true);
	ui.ZRelativeSpinLabel->setEnabled(true);

	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setZType(ReferenceVariable::RelativeReference);
}

void ReferenceSetupDialog::on_ZDataSetCombo_currentIndexChanged(int index)
{
	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setZIndex(index);
	referenceTarget_->setZDataSetName(ui.ZDataSetCombo->currentText());

	updateLabels();
}

void ReferenceSetupDialog::on_ZRelativeSpin_valueChanged(int value)
{
	if (refreshing_ || (!referenceTarget_)) return;

	referenceTarget_->setZOffset(value-1);

	updateLabels();
}

// Update labels
void ReferenceSetupDialog::updateLabels()
{
	if (refreshing_ || (!referenceTarget_)) return;

	const Array<double>& abscissa = referenceTarget_->sourceCollection()->displayAbscissa();
	ui.XFixedLabel->setText("(X = " + QString::number(abscissa.value(ui.XFixedSpin->value())) + ")");

	// Z Source
	DataSet* dataSet = referenceTarget_->sourceCollection()->nDataSets() == 0 ? NULL : referenceTarget_->sourceCollection()->dataSet(ui.ZDataSetCombo->currentIndex());
	ui.ZFixedLabel->setText("(Z = " + (dataSet ? QString::number(dataSet->data().z()) + ")" : "?)"));
}
