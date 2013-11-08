/*
	*** Main Window - Transform Functions 
	*** src/gui/uchroma_transform.cpp
	Copyright T. Youngs 2013

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
	if (refreshing_) return false;
	transformEnabled_[axis] = enabled;
	calculateTransformLimits();
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;

}

bool UChromaWindow::transformEquationChanged(int axis, QString equation)
{
	if (refreshing_) return false;
	transforms_[axis].setEquation(equation);
	calculateTransformLimits();
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool UChromaWindow::transformShiftChanged(int axis, bool pre, double value)
{
	if (refreshing_) return false;
	if (pre) preTransformShift_[axis] = value;
	else postTransformShift_[axis] = value;
	calculateTransformLimits();
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool UChromaWindow::transformLimitChanged(int axis, bool minLim, double value)
{
	if (refreshing_) return false;
	if (minLim) limitMin_[axis] = value;
	else limitMax_[axis] = value;
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool UChromaWindow::transformLimitSetExtreme(int axis, bool minLim)
{
	if (refreshing_) return false;
	if (minLim) limitMin_[axis] = transformMin_[axis];
	else limitMax_[axis] = transformMax_[axis];
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool UChromaWindow::transformInterpolateChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	interpolate_[axis] = checked;
	setAsModified();
	updateSurface();
	return true;
}

bool UChromaWindow::transformInterpolateStepChanged(int axis, double step)
{
	if (refreshing_) return false;
	interpolationStep_[axis] = step;
	setAsModified();
	updateSurface();
	return true;
}

bool UChromaWindow::transformInterpolateConstrainChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	interpolateConstrained_[axis] = checked;
	setAsModified();
	updateSurface();
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
	transformEquationChanged(0, text);
}

void UChromaWindow::on_TransformZEquationEdit_textEdited(QString text)
{
	transformEquationChanged(0, text);
}

void UChromaWindow::on_TransformXPreShiftSpin_valueChanged(double value)
{
	transformShiftChanged(0, true, value);
}

void UChromaWindow::on_TransformYPreShiftSpin_valueChanged(double value)
{
	transformShiftChanged(1, true, value);
}

void UChromaWindow::on_TransformZPreShiftSpin_valueChanged(double value)
{
	transformShiftChanged(2, true, value);
}

void UChromaWindow::on_TransformXPostShiftSpin_valueChanged(double value)
{
	transformShiftChanged(0, false, value);
}

void UChromaWindow::on_TransformYPostShiftSpin_valueChanged(double value)
{
	transformShiftChanged(1, false, value);
}

void UChromaWindow::on_TransformZPostShiftSpin_valueChanged(double value)
{
	transformShiftChanged(2, false, value);
}

void UChromaWindow::on_LimitXMinSpin_valueChanged(double value)
{
	transformLimitChanged(0, true, value);
}

void UChromaWindow::on_LimitYMinSpin_valueChanged(double value)
{
	transformLimitChanged(1, true, value);
}

void UChromaWindow::on_LimitZMinSpin_valueChanged(double value)
{
	transformLimitChanged(2, true, value);
}

void UChromaWindow::on_LimitXMinSetMinimumButton_clicked(bool checked)
{
	transformLimitSetExtreme(0, true);
}

void UChromaWindow::on_LimitYMinSetMinimumButton_clicked(bool checked)
{
	transformLimitSetExtreme(1, true);
}

void UChromaWindow::on_LimitZMinSetMinimumButton_clicked(bool checked)
{
	transformLimitSetExtreme(2, true);
}

void UChromaWindow::on_LimitXMaxSpin_valueChanged(double value)
{
	transformLimitChanged(0, false, value);
}

void UChromaWindow::on_LimitYMaxSpin_valueChanged(double value)
{
	transformLimitChanged(1, false, value);
}

void UChromaWindow::on_LimitZMaxSpin_valueChanged(double value)
{
	transformLimitChanged(2, false, value);
}

void UChromaWindow::on_LimitXMaxSetMaximumButton_clicked(bool checked)
{
	transformLimitSetExtreme(0, false);
}

void UChromaWindow::on_LimitYMaxSetMaximumButton_clicked(bool checked)
{
	transformLimitSetExtreme(1, false);
}

void UChromaWindow::on_LimitZMaxSetMaximumButton_clicked(bool checked)
{
	transformLimitSetExtreme(2, false);
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
void UChromaWindow::updateTransformTab()
{
	refreshing_ = true;

	// Transform type / value
	ui.TransformXCheck->setChecked(transformEnabled_[0]);
	ui.TransformYCheck->setChecked(transformEnabled_[1]);
	ui.TransformZCheck->setChecked(transformEnabled_[2]);
	ui.TransformXEquationEdit->setText(transforms_[0].text());
	ui.TransformYEquationEdit->setText(transforms_[1].text());
	ui.TransformZEquationEdit->setText(transforms_[2].text());

	// Shifts
	ui.TransformXPreShiftSpin->setValue(preTransformShift_.x);
	ui.TransformYPreShiftSpin->setValue(preTransformShift_.y);
	ui.TransformZPreShiftSpin->setValue(preTransformShift_.z);
	ui.TransformXPostShiftSpin->setValue(postTransformShift_.x);
	ui.TransformYPostShiftSpin->setValue(postTransformShift_.y);
	ui.TransformZPostShiftSpin->setValue(postTransformShift_.z);

	// Limits
	ui.LimitXMinSpin->setRange(transformMin_.x, transformMax_.x);
	ui.LimitYMinSpin->setRange(transformMin_.y, transformMax_.y);
	ui.LimitZMinSpin->setRange(transformMin_.z, transformMax_.z);
	ui.LimitXMinSpin->setSingleStep((transformMax_.x-transformMin_.x)*0.01);
	ui.LimitYMinSpin->setSingleStep((transformMax_.y-transformMin_.y)*0.01);
	ui.LimitZMinSpin->setSingleStep((transformMax_.z-transformMin_.z)*0.01);

	ui.LimitXMaxSpin->setRange(transformMin_.x, transformMax_.x);
	ui.LimitYMaxSpin->setRange(transformMin_.y, transformMax_.y);
	ui.LimitZMaxSpin->setRange(transformMin_.z, transformMax_.z);
	ui.LimitXMaxSpin->setSingleStep((transformMax_.x-transformMin_.x)*0.01);
	ui.LimitYMaxSpin->setSingleStep((transformMax_.y-transformMin_.y)*0.01);
	ui.LimitZMaxSpin->setSingleStep((transformMax_.z-transformMin_.z)*0.01);
	ui.LimitXMinSpin->setValue(limitMin_.x);
	ui.LimitYMinSpin->setValue(limitMin_.y);
	ui.LimitZMinSpin->setValue(limitMin_.z);
	ui.LimitXMaxSpin->setValue(limitMax_.x);
	ui.LimitYMaxSpin->setValue(limitMax_.y);
	ui.LimitZMaxSpin->setValue(limitMax_.z);
	ui.LimitXMinLabel->setText(QString::number(transformMin_.x));
	ui.LimitXMaxLabel->setText(QString::number(transformMax_.x));
	ui.LimitYMinLabel->setText(QString::number(transformMin_.y));
	ui.LimitYMaxLabel->setText(QString::number(transformMax_.y));
	ui.LimitZMinLabel->setText(QString::number(transformMin_.z));
	ui.LimitZMaxLabel->setText(QString::number(transformMax_.z));

	// Interpolation
	ui.TransformXInterpolateCheck->setChecked(interpolate_.x);
	ui.TransformXInterpolateConstrainCheck->setChecked(interpolateConstrained_.x);
	ui.TransformXInterpolateStepSpin->setValue(interpolationStep_.x);
	ui.TransformZInterpolateCheck->setChecked(interpolate_.z);
	ui.TransformZInterpolateConstrainCheck->setChecked(interpolateConstrained_.z);
	ui.TransformZInterpolateStepSpin->setValue(interpolationStep_.z);

	refreshing_ = false;
}
