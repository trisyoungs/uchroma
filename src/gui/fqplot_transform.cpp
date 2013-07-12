/*
	*** Main Window - Transform Functions 
	*** src/gui/fqplot_transform.cpp
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/fqplot.h"
#include "templates/reflist.h"

bool FQPlotWindow::transformTypeChanged(int axis, int index)
{
	if (refreshing_) return false;
	transformType_[axis] = (DataTransform) index;
	calculateTransformLimits();
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool FQPlotWindow::transformValueChanged(int axis, double value)
{
	if (refreshing_) return false;
	transformValue_[axis] = value;
	calculateTransformLimits();
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool FQPlotWindow::transformShiftChanged(int axis, bool pre, double value)
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

bool FQPlotWindow::transformLimitChanged(int axis, bool minLim, double value)
{
	if (refreshing_) return false;
	if (minLim) limitMin_[axis] = value;
	else limitMax_[axis] = value;
// 	calculateTransformLimits();
	setAsModified();
// 	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool FQPlotWindow::transformLimitSetExtreme(int axis, bool minLim)
{
	if (refreshing_) return false;
	if (minLim) limitMin_[axis] = transformMin_[axis];
	else limitMax_[axis] = transformMax_[axis];
// 	calculateTransformLimits();
	setAsModified();
// 	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool FQPlotWindow::transformInterpolateChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	interpolate_[axis] = checked;
	setAsModified();
	updateSurface();
	return true;
}

bool FQPlotWindow::transformInterpolateStepChanged(int axis, double step)
{
	if (refreshing_) return false;
	interpolationStep_[axis] = step;
	setAsModified();
	updateSurface();
	return true;
}

bool FQPlotWindow::transformInterpolateConstrainChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	interpolateConstrained_[axis] = checked;
	setAsModified();
	updateSurface();
	return true;
}

void FQPlotWindow::on_TransformXTypeCombo_currentIndexChanged(int index)
{
	if (transformTypeChanged(0, index)) ui.TransformXValueSpin->setEnabled(index < 2);
}

void FQPlotWindow::on_TransformYTypeCombo_currentIndexChanged(int index)
{
	if (transformTypeChanged(1, index)) ui.TransformYValueSpin->setEnabled(index < 2);
}

void FQPlotWindow::on_TransformZTypeCombo_currentIndexChanged(int index)
{
	if (transformTypeChanged(2, index)) ui.TransformZValueSpin->setEnabled(index < 2);
}

void FQPlotWindow::on_TransformXValueSpin_valueChanged(double value)
{
	transformValueChanged(0, value);
}

void FQPlotWindow::on_TransformYValueSpin_valueChanged(double value)
{
	transformValueChanged(1, value);
}

void FQPlotWindow::on_TransformZValueSpin_valueChanged(double value)
{
	transformValueChanged(2, value);
}

void FQPlotWindow::on_TransformXPreShiftSpin_valueChanged(double value)
{
	transformShiftChanged(0, true, value);
}

void FQPlotWindow::on_TransformYPreShiftSpin_valueChanged(double value)
{
	transformShiftChanged(1, true, value);
}

void FQPlotWindow::on_TransformZPreShiftSpin_valueChanged(double value)
{
	transformShiftChanged(2, true, value);
}

void FQPlotWindow::on_TransformXPostShiftSpin_valueChanged(double value)
{
	transformShiftChanged(0, false, value);
}

void FQPlotWindow::on_TransformYPostShiftSpin_valueChanged(double value)
{
	transformShiftChanged(1, false, value);
}

void FQPlotWindow::on_TransformZPostShiftSpin_valueChanged(double value)
{
	transformShiftChanged(2, false, value);
}

void FQPlotWindow::on_LimitXMinSpin_valueChanged(double value)
{
	transformLimitChanged(0, true, value);
}

void FQPlotWindow::on_LimitYMinSpin_valueChanged(double value)
{
	transformLimitChanged(1, true, value);
}

void FQPlotWindow::on_LimitZMinSpin_valueChanged(double value)
{
	transformLimitChanged(2, true, value);
}

void FQPlotWindow::on_LimitXMinSetMinimumButton_clicked(bool checked)
{
	transformLimitSetExtreme(0, true);
}

void FQPlotWindow::on_LimitYMinSetMinimumButton_clicked(bool checked)
{
	transformLimitSetExtreme(1, true);
}

void FQPlotWindow::on_LimitZMinSetMinimumButton_clicked(bool checked)
{
	transformLimitSetExtreme(2, true);
}

void FQPlotWindow::on_LimitXMaxSpin_valueChanged(double value)
{
	transformLimitChanged(0, false, value);
}

void FQPlotWindow::on_LimitYMaxSpin_valueChanged(double value)
{
	transformLimitChanged(1, false, value);
}

void FQPlotWindow::on_LimitZMaxSpin_valueChanged(double value)
{
	transformLimitChanged(2, false, value);
}

void FQPlotWindow::on_LimitXMaxSetMaximumButton_clicked(bool checked)
{
	transformLimitSetExtreme(0, false);
}

void FQPlotWindow::on_LimitYMaxSetMaximumButton_clicked(bool checked)
{
	transformLimitSetExtreme(1, false);
}

void FQPlotWindow::on_LimitZMaxSetMaximumButton_clicked(bool checked)
{
	transformLimitSetExtreme(2, false);
}

void FQPlotWindow::on_TransformXInterpolateCheck_clicked(bool checked)
{
	transformInterpolateChanged(0, checked);
	ui.TransformXInterpolateOptions->setEnabled(checked);
}

void FQPlotWindow::on_TransformXInterpolateStepSpin_valueChanged(double value)
{
	transformInterpolateStepChanged(0, value);
}

void FQPlotWindow::on_TransformXInterpolateConstrainCheck_clicked(bool checked)
{
	transformInterpolateConstrainChanged(0, checked);
}

void FQPlotWindow::on_TransformZInterpolateCheck_clicked(bool checked)
{
	transformInterpolateChanged(2, checked);
	ui.TransformZInterpolateOptions->setEnabled(checked);
}

void FQPlotWindow::on_TransformZInterpolateStepSpin_valueChanged(double value)
{
	transformInterpolateStepChanged(2, value);
}

void FQPlotWindow::on_TransformZInterpolateConstrainCheck_clicked(bool checked)
{
	transformInterpolateConstrainChanged(2, checked);
}

// Update Transform tab
void FQPlotWindow::updateTransformTab()
{
	refreshing_ = true;

	// Transform type / value
	ui.TransformXTypeCombo->setCurrentIndex(transformType_[0]);
	ui.TransformYTypeCombo->setCurrentIndex(transformType_[1]);
	ui.TransformZTypeCombo->setCurrentIndex(transformType_[2]);
	ui.TransformXValueSpin->setEnabled(transformType_[0] < 2);
	ui.TransformYValueSpin->setEnabled(transformType_[1] < 2);
	ui.TransformZValueSpin->setEnabled(transformType_[2] < 2);
	ui.TransformXValueSpin->setValue(transformValue_.x);
	ui.TransformYValueSpin->setValue(transformValue_.y);
	ui.TransformZValueSpin->setValue(transformValue_.z);

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
