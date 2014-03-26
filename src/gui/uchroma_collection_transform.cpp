/*
	*** Main Window - Collection Transform Functions 
	*** src/gui/uchroma_collection_transform.cpp
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

#include "gui/uchroma.h"
#include "templates/reflist.h"


bool UChromaWindow::transformEnabledChanged(int axis, bool enabled)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return false;

	currentCollection_->setTransformEnabled(axis, enabled);
	currentCollection_->updateDataTransforms();

	// Update related tabs and main display
	setAsModified();
	updateCollectionTransformTab();
	updateAxesTab();
	updateDisplay();

	return true;
}

bool UChromaWindow::transformEquationChanged(int axis, QString equation)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return false;

	currentCollection_->setTransformEquation(axis, equation);
	currentCollection_->updateDataTransforms();

	// Update related tabs and main display
	setAsModified();
	updateCollectionTransformTab();
	updateAxesTab();
	updateDisplay();

	return true;
}

bool UChromaWindow::transformInterpolateChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	currentCollection_->setInterpolate(axis, checked);

	// Update display
	setAsModified();
	updateDisplay();

	return true;
}

bool UChromaWindow::transformInterpolateStepChanged(int axis, double step)
{
	if (refreshing_) return false;
	currentCollection_->setInterpolationStep(axis, step);

	// Update display
	setAsModified();
	updateDisplay();

	return true;
}

bool UChromaWindow::transformInterpolateConstrainChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	currentCollection_->setInterpolateConstrained(axis, checked);

	// Update display
	setAsModified();
	updateDisplay();

	return true;
}

void UChromaWindow::on_TransformXCheck_clicked(bool checked)
{
	transformEnabledChanged(0, checked);
}

void UChromaWindow::on_TransformYCheck_clicked(bool checked)
{
	transformEnabledChanged(1, checked);
}

void UChromaWindow::on_TransformZCheck_clicked(bool checked)
{
	transformEnabledChanged(2, checked);
}

void UChromaWindow::on_TransformXEquationEdit_textEdited(QString text)
{
	transformEquationChanged(0, text);
}

void UChromaWindow::on_TransformYEquationEdit_textEdited(QString text)
{
	transformEquationChanged(1, text);
}

void UChromaWindow::on_TransformZEquationEdit_textEdited(QString text)
{
	transformEquationChanged(2, text);
}


void UChromaWindow::on_TransformXInterpolateCheck_clicked(bool checked)
{
	transformInterpolateChanged(0, checked);
	ui.TransformXInterpolateOptions->setEnabled(checked);
}

void UChromaWindow::on_TransformXInterpolateStepSpin_valueChanged(double value)
{
	transformInterpolateStepChanged(0, value);
}

void UChromaWindow::on_TransformXInterpolateConstrainCheck_clicked(bool checked)
{
	transformInterpolateConstrainChanged(0, checked);
}

void UChromaWindow::on_TransformZInterpolateCheck_clicked(bool checked)
{
	transformInterpolateChanged(2, checked);
	ui.TransformZInterpolateOptions->setEnabled(checked);
}

void UChromaWindow::on_TransformZInterpolateStepSpin_valueChanged(double value)
{
	transformInterpolateStepChanged(2, value);
}

void UChromaWindow::on_TransformZInterpolateConstrainCheck_clicked(bool checked)
{
	transformInterpolateConstrainChanged(2, checked);
}

// Update Transform tab
void UChromaWindow::updateCollectionTransformTab()
{
	// Check for invalid Collection
	if (!currentCollection_) return;

	refreshing_ = true;

	// Transform type / value
	ui.TransformXCheck->setChecked(currentCollection_->transformEnabled(0));
	ui.TransformYCheck->setChecked(currentCollection_->transformEnabled(1));
	ui.TransformZCheck->setChecked(currentCollection_->transformEnabled(2));
	ui.TransformXEquationEdit->setEnabled(currentCollection_->transformEnabled(0));
	ui.TransformYEquationEdit->setEnabled(currentCollection_->transformEnabled(1));
	ui.TransformZEquationEdit->setEnabled(currentCollection_->transformEnabled(2));
	ui.TransformXEquationEdit->setText(currentCollection_->transformEquation(0));
	ui.TransformYEquationEdit->setText(currentCollection_->transformEquation(1));
	ui.TransformZEquationEdit->setText(currentCollection_->transformEquation(2));
	QPalette redText = ui.TransformXCheck->palette();
	redText.setColor(QPalette::Text, Qt::red);
	ui.TransformXEquationEdit->setPalette(currentCollection_->transformEquationValid(0) ? ui.TransformXCheck->palette() : redText);
	ui.TransformYEquationEdit->setPalette(currentCollection_->transformEquationValid(1) ? ui.TransformXCheck->palette() : redText);
	ui.TransformZEquationEdit->setPalette(currentCollection_->transformEquationValid(2) ? ui.TransformXCheck->palette() : redText);

	// Interpolation
	ui.TransformXInterpolateCheck->setChecked(currentCollection_->interpolate(0));
	ui.TransformXInterpolateConstrainCheck->setChecked(currentCollection_->interpolateConstrained(0));
	ui.TransformXInterpolateStepSpin->setValue(currentCollection_->interpolationStep(0));
	ui.TransformZInterpolateCheck->setChecked(currentCollection_->interpolate(2));
	ui.TransformZInterpolateConstrainCheck->setChecked(currentCollection_->interpolateConstrained(2));
	ui.TransformZInterpolateStepSpin->setValue(currentCollection_->interpolationStep(2));

	refreshing_ = false;
}
