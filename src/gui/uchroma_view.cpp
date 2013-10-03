/*
	*** Main Window - View Functions 
	*** src/gui/uchroma_view.cpp
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

bool UChromaWindow::viewAxisInvertChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	axisInvert_[axis] = checked;
	setAsModified();
	updateSurface();
	return true;
}

bool UChromaWindow::viewAxisLogarithmicChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	axisLogarithmic_[axis] = checked;
	setAsModified();
	updateSurface(true);
	return true;
}

bool UChromaWindow::viewAxisVisibleChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	axisVisible_[axis] = checked;
	setAsModified();
	updateSurface(false);
	return true;
}

bool UChromaWindow::viewAxisStretchChanged(int axis, double value)
{
	if (refreshing_) return false;
	axisStretch_[axis] = value;
	setAsModified();
	updateSurface();
	return true;
}

bool UChromaWindow::viewAxisCrossChanged(int axis, int dir, double value)
{
	if (refreshing_) return false;
	axisPosition_[axis].set(dir,value);
	setAsModified();
	updateSurface(false);
	return true;
}

bool UChromaWindow::viewAxisCrossSet(int axis, int dir, int type)
{
	if (refreshing_) return false;
	if (type == -1) axisPosition_[axis].set(dir, limitMin_[dir]);
	else if ((type == 0) && (limitMin_[dir] <= 0.0) && (limitMax_[dir] >= 0.0)) axisPosition_[axis].set(dir, 0.0);
	else if (type == 1) axisPosition_[axis].set(dir, limitMax_[dir]);
	else return false;
	setAsModified();
	updateViewTab();
	updateSurface(false);
	return true;
}

bool UChromaWindow::viewAxisTicksChanged(int axis, bool start, double value)
{
	if (refreshing_) return false;
	if (start) axisFirstTick_[axis] = value;
	else axisTickDelta_[axis] = value;
	setAsModified();
	updateSurface(false);
	return true;
}

bool UChromaWindow::viewAxisOrientationChanged(int axis, int dir, bool direction, double value)
{
	if (refreshing_) return false;
	if (direction) axisLabelDirection_[axis].set(dir, value);
	else axisLabelUp_[axis].set(dir, value);
	setAsModified();
	updateSurface(false);
	return true;
}

bool UChromaWindow::viewAxisMinorTicksChanged(int axis, int value)
{
	if (refreshing_) return false;
	axisMinorTicks_[axis] = value;
	setAsModified();
	updateSurface(false);
	return true;
}

bool UChromaWindow::viewAxisRotationChanged(int axis, int rotation)
{
	if (refreshing_) return false;
	axisLabelRotation_[axis] = rotation;
	setAsModified();
	updateSurface(false);
	return true;
}

void UChromaWindow::on_ViewLabelScaleSpin_valueChanged(double value)
{
	if (refreshing_) return;
	labelScale_ = value;
	ui.MainView->setLabelScale(labelScale_);
	setAsModified();
	updateSurface(false);
}

void UChromaWindow::on_ViewTitleScaleSpin_valueChanged(double value)
{
	if (refreshing_) return;
	titleScale_ = value;
	ui.MainView->setTitleScale(titleScale_);
	setAsModified();
	updateSurface(false);
}

// X Axis

void UChromaWindow::on_ViewXAxisInvertCheck_clicked(bool checked)
{
	viewAxisInvertChanged(0, checked);
}

void UChromaWindow::on_ViewXAxisLogarithmicCheck_clicked(bool checked)
{
	viewAxisLogarithmicChanged(0, checked);
}

void UChromaWindow::on_ViewXAxisVisibleCheck_clicked(bool checked)
{
	viewAxisVisibleChanged(0, checked);
}

void UChromaWindow::on_ViewXAxisStretchSpin_valueChanged(double value)
{
	viewAxisStretchChanged(0, value);
}

void UChromaWindow::on_ViewXAxisCrossAtYSpin_valueChanged(double value)
{
	viewAxisCrossChanged(0, 1, value);
}

void UChromaWindow::on_ViewXAxisCrossAtZSpin_valueChanged(double value)
{
	viewAxisCrossChanged(0, 2, value);
}

void UChromaWindow::on_ViewXAxisCrossAtYSetMinimumButton_clicked(bool checked)
{
	viewAxisCrossSet(0, 1, -1);
}

void UChromaWindow::on_ViewXAxisCrossAtYSetZeroButton_clicked(bool checked)
{
	viewAxisCrossSet(0, 1, 0);
}

void UChromaWindow::on_ViewXAxisCrossAtYSetMaximumButton_clicked(bool checked)
{
	viewAxisCrossSet(0, 1, 1);
}

void UChromaWindow::on_ViewXAxisCrossAtZSetMinimumButton_clicked(bool checked)
{
	viewAxisCrossSet(0, 2, -1);
}

void UChromaWindow::on_ViewXAxisCrossAtZSetZeroButton_clicked(bool checked)
{
	viewAxisCrossSet(0, 2, 0);
}

void UChromaWindow::on_ViewXAxisCrossAtZSetMaximumButton_clicked(bool checked)
{
	viewAxisCrossSet(0, 2, 1);
}

void UChromaWindow::on_ViewXAxisAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicks_.x = checked;
	updateViewTab();
	updateSurface(false);
}

void UChromaWindow::on_ViewXAxisTicksStartSpin_valueChanged(double value)
{
	viewAxisTicksChanged(0, true, value);
}

void UChromaWindow::on_ViewXAxisTicksDeltaSpin_valueChanged(double value)
{
	viewAxisTicksChanged(0, false, value);
}

void UChromaWindow::on_ViewXAxisMinorTicksSpin_valueChanged(int value)
{
	viewAxisMinorTicksChanged(0, value);
}

void UChromaWindow::on_ViewXAxisDirectionXSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(0, 0, true, value);
}

void UChromaWindow::on_ViewXAxisDirectionYSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(0, 1, true, value);
}

void UChromaWindow::on_ViewXAxisDirectionZSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(0, 2, true, value);
}

void UChromaWindow::on_ViewXAxisUpXSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(0, 0, false, value);
}

void UChromaWindow::on_ViewXAxisUpYSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(0, 1, false, value);
}

void UChromaWindow::on_ViewXAxisUpZSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(0, 2, false, value);
}

void UChromaWindow::on_ViewXAxisRotationSpin_valueChanged(int value)
{
	viewAxisRotationChanged(0, value);
}

// Y Axis

void UChromaWindow::on_ViewYAxisInvertCheck_clicked(bool checked)
{
	viewAxisInvertChanged(1, checked);
}

void UChromaWindow::on_ViewYAxisLogarithmicCheck_clicked(bool checked)
{
	viewAxisLogarithmicChanged(1, checked);
}

void UChromaWindow::on_ViewYAxisVisibleCheck_clicked(bool checked)
{
	viewAxisVisibleChanged(1, checked);
}

void UChromaWindow::on_ViewYAxisStretchSpin_valueChanged(double value)
{
	viewAxisStretchChanged(1, value);
}

void UChromaWindow::on_ViewYAxisCrossAtXSpin_valueChanged(double value)
{
	viewAxisCrossChanged(1, 0, value);
}

void UChromaWindow::on_ViewYAxisCrossAtZSpin_valueChanged(double value)
{
	viewAxisCrossChanged(1, 2, value);
}

void UChromaWindow::on_ViewYAxisCrossAtXSetMinimumButton_clicked(bool checked)
{
	viewAxisCrossSet(1, 0, -1);
}

void UChromaWindow::on_ViewYAxisCrossAtXSetZeroButton_clicked(bool checked)
{
	viewAxisCrossSet(1, 0, 0);
}

void UChromaWindow::on_ViewYAxisCrossAtXSetMaximumButton_clicked(bool checked)
{
	viewAxisCrossSet(1, 0, 1);
}

void UChromaWindow::on_ViewYAxisCrossAtZSetMinimumButton_clicked(bool checked)
{
	viewAxisCrossSet(1, 2, -1);
}

void UChromaWindow::on_ViewYAxisCrossAtZSetZeroButton_clicked(bool checked)
{
	viewAxisCrossSet(1, 2, 0);
}

void UChromaWindow::on_ViewYAxisCrossAtZSetMaximumButton_clicked(bool checked)
{
	viewAxisCrossSet(1, 2, 1);
}

void UChromaWindow::on_ViewYAxisAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicks_.y = checked;
	updateViewTab();
	updateSurface(false);
}

void UChromaWindow::on_ViewYAxisTicksStartSpin_valueChanged(double value)
{
	viewAxisTicksChanged(1, true, value);
}

void UChromaWindow::on_ViewYAxisTicksDeltaSpin_valueChanged(double value)
{
	viewAxisTicksChanged(1, false, value);
}

void UChromaWindow::on_ViewYAxisMinorTicksSpin_valueChanged(int value)
{
	viewAxisMinorTicksChanged(1, value);
}

void UChromaWindow::on_ViewYAxisDirectionXSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(1, 0, true, value);
}

void UChromaWindow::on_ViewYAxisDirectionYSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(1, 1, true, value);
}

void UChromaWindow::on_ViewYAxisDirectionZSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(1, 2, true, value);
}

void UChromaWindow::on_ViewYAxisUpXSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(1, 0, false, value);
}

void UChromaWindow::on_ViewYAxisUpYSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(1, 1, false, value);
}

void UChromaWindow::on_ViewYAxisUpZSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(1, 2, false, value);
}

void UChromaWindow::on_ViewYAxisRotationSpin_valueChanged(int value)
{
	viewAxisRotationChanged(1, value);
}

// Z Axis

void UChromaWindow::on_ViewZAxisInvertCheck_clicked(bool checked)
{
	viewAxisInvertChanged(2, checked);
}

void UChromaWindow::on_ViewZAxisLogarithmicCheck_clicked(bool checked)
{
	viewAxisLogarithmicChanged(2, checked);
}

void UChromaWindow::on_ViewZAxisVisibleCheck_clicked(bool checked)
{
	viewAxisVisibleChanged(2, checked);
}

void UChromaWindow::on_ViewZAxisStretchSpin_valueChanged(double value)
{
	viewAxisStretchChanged(2, value);
}

void UChromaWindow::on_ViewZAxisCrossAtXSpin_valueChanged(double value)
{
	viewAxisCrossChanged(2, 0, value);
}

void UChromaWindow::on_ViewZAxisCrossAtYSpin_valueChanged(double value)
{
	viewAxisCrossChanged(2, 1, value);
}

void UChromaWindow::on_ViewZAxisCrossAtXSetMinimumButton_clicked(bool checked)
{
	viewAxisCrossSet(2, 0, -1);
}

void UChromaWindow::on_ViewZAxisCrossAtXSetZeroButton_clicked(bool checked)
{
	viewAxisCrossSet(2, 0, 0);
}

void UChromaWindow::on_ViewZAxisCrossAtXSetMaximumButton_clicked(bool checked)
{
	viewAxisCrossSet(2, 0, 1);
}

void UChromaWindow::on_ViewZAxisCrossAtYSetMinimumButton_clicked(bool checked)
{
	viewAxisCrossSet(2, 1, -1);
}

void UChromaWindow::on_ViewZAxisCrossAtYSetZeroButton_clicked(bool checked)
{
	viewAxisCrossSet(2, 1, 0);
}

void UChromaWindow::on_ViewZAxisCrossAtYSetMaximumButton_clicked(bool checked)
{
	viewAxisCrossSet(2, 1, 1);
}

void UChromaWindow::on_ViewZAxisAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicks_.z = checked;
	updateViewTab();
	updateSurface(false);
}

void UChromaWindow::on_ViewZAxisTicksStartSpin_valueChanged(double value)
{
	viewAxisTicksChanged(2, true, value);
}

void UChromaWindow::on_ViewZAxisTicksDeltaSpin_valueChanged(double value)
{
	viewAxisTicksChanged(2, false, value);
}

void UChromaWindow::on_ViewZAxisMinorTicksSpin_valueChanged(int value)
{
	viewAxisMinorTicksChanged(2, value);
}

void UChromaWindow::on_ViewZAxisDirectionXSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(2, 0, true, value);
}

void UChromaWindow::on_ViewZAxisDirectionYSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(2, 1, true, value);
}

void UChromaWindow::on_ViewZAxisDirectionZSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(2, 2, true, value);
}

void UChromaWindow::on_ViewZAxisUpXSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(2, 0, false, value);
}

void UChromaWindow::on_ViewZAxisUpYSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(2, 1, false, value);
}

void UChromaWindow::on_ViewZAxisUpZSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(2, 2, false, value);
}

void UChromaWindow::on_ViewZAxisRotationSpin_valueChanged(int value)
{
	viewAxisRotationChanged(2, value);
}

/*
 * Extras Tab
 */

void UChromaWindow::on_ViewBoundingBoxNoneRadio_clicked(bool checked)
{
	if (refreshing_) return;
	boundingBox_ = UChromaWindow::NoBox;
	updateSurface(false);
}

void UChromaWindow::on_ViewBoundingBoxPlaneRadio_clicked(bool checked)
{
	if (refreshing_) return;
	boundingBox_ = UChromaWindow::PlaneBox;
	updateSurface(false);
}

void UChromaWindow::on_ViewBoundingBoxCubeRadio_clicked(bool checked)
{
	if (refreshing_) return;
	boundingBox_ = UChromaWindow::CubeBox;
	updateSurface(false);
}

void UChromaWindow::on_ViewBoundingBoxPlaneYSpin_valueChanged(double value)
{
	if (refreshing_) return;
	boundingBoxPlaneY_ = value;
	updateSurface(false);
}

void UChromaWindow::on_ViewBoundingBoxPlaneYSetMinimumButton_clicked(bool checked)
{
	ui.ViewBoundingBoxPlaneYSpin->setValue(limitMin_.y);
}

void UChromaWindow::on_ViewBoundingBoxPlaneYSetZeroButton_clicked(bool checked)
{
	ui.ViewBoundingBoxPlaneYSpin->setValue(0.0);
}

void UChromaWindow::on_ViewBoundingBoxPlaneYSetMaximumButton_clicked(bool checked)
{
	ui.ViewBoundingBoxPlaneYSpin->setValue(limitMax_.y);
}

/*
 * Update
 */
void UChromaWindow::updateViewTab()
{
	refreshing_ = true;
	
	// Label scales
	ui.ViewLabelScaleSpin->setValue(labelScale_);
	ui.ViewTitleScaleSpin->setValue(titleScale_);

	// Invert / Visible / Logarithmic
	ui.ViewXAxisInvertCheck->setChecked(axisInvert_.x);
	ui.ViewYAxisInvertCheck->setChecked(axisInvert_.y);
	ui.ViewZAxisInvertCheck->setChecked(axisInvert_.z);
	ui.ViewXAxisVisibleCheck->setChecked(axisVisible_.x);
	ui.ViewYAxisVisibleCheck->setChecked(axisVisible_.y);
	ui.ViewZAxisVisibleCheck->setChecked(axisVisible_.z);
	ui.ViewXAxisLogarithmicCheck->setChecked(axisLogarithmic_.x);
	ui.ViewYAxisLogarithmicCheck->setChecked(axisLogarithmic_.y);
	ui.ViewZAxisLogarithmicCheck->setChecked(axisLogarithmic_.z);

	// Axis Stretch factors
	ui.ViewXAxisStretchSpin->setValue(axisStretch_.x);
	ui.ViewYAxisStretchSpin->setValue(axisStretch_.y);
	ui.ViewZAxisStretchSpin->setValue(axisStretch_.z);

	// Axis positions
	// -- X
	ui.ViewXAxisCrossAtYSpin->setValue(axisPosition_[0].y);
	ui.ViewXAxisCrossAtZSpin->setValue(axisPosition_[0].z);
	// -- Y
	ui.ViewYAxisCrossAtXSpin->setValue(axisPosition_[1].x);
	ui.ViewYAxisCrossAtZSpin->setValue(axisPosition_[1].z);
	// -- Z
	ui.ViewZAxisCrossAtXSpin->setValue(axisPosition_[2].x);
	ui.ViewZAxisCrossAtYSpin->setValue(axisPosition_[2].y);

	// AxisTicks
	// -- X
	ui.ViewXAxisAutoTicksCheck->setChecked(axisAutoTicks_.x);
	ui.ViewXAxisTicksWidget->setEnabled(!axisAutoTicks_.x);
	ui.ViewXAxisTicksStartSpin->setValue(axisFirstTick_.x);
	ui.ViewXAxisTicksDeltaSpin->setValue(axisTickDelta_.x);
	ui.ViewXAxisMinorTicksSpin->setValue(axisMinorTicks_.x);
	// -- Y
	ui.ViewYAxisAutoTicksCheck->setChecked(axisAutoTicks_.y);
	ui.ViewYAxisTicksWidget->setEnabled(!axisAutoTicks_.y);
	ui.ViewYAxisTicksStartSpin->setValue(axisFirstTick_.y);
	ui.ViewYAxisTicksDeltaSpin->setValue(axisTickDelta_.y);
	ui.ViewYAxisMinorTicksSpin->setValue(axisMinorTicks_.y);
	// -- Z
	ui.ViewZAxisAutoTicksCheck->setChecked(axisAutoTicks_.z);
	ui.ViewZAxisTicksWidget->setEnabled(!axisAutoTicks_.z);
	ui.ViewZAxisTicksStartSpin->setValue(axisFirstTick_.z);
	ui.ViewZAxisTicksDeltaSpin->setValue(axisTickDelta_.z);
	ui.ViewZAxisMinorTicksSpin->setValue(axisMinorTicks_.z);

	// Orientation
	// -- X
	ui.ViewXAxisDirectionXSpin->setValue(axisLabelDirection_[0].x);
	ui.ViewXAxisDirectionYSpin->setValue(axisLabelDirection_[0].y);
	ui.ViewXAxisDirectionZSpin->setValue(axisLabelDirection_[0].z);
	ui.ViewXAxisUpXSpin->setValue(axisLabelUp_[0].x);
	ui.ViewXAxisUpYSpin->setValue(axisLabelUp_[0].y);
	ui.ViewXAxisUpZSpin->setValue(axisLabelUp_[0].z);
	ui.ViewXAxisRotationSpin->setValue(axisLabelRotation_.x);
	// -- Y
	ui.ViewYAxisDirectionXSpin->setValue(axisLabelDirection_[1].x);
	ui.ViewYAxisDirectionYSpin->setValue(axisLabelDirection_[1].y);
	ui.ViewYAxisDirectionZSpin->setValue(axisLabelDirection_[1].z);
	ui.ViewYAxisUpXSpin->setValue(axisLabelUp_[1].x);
	ui.ViewYAxisUpYSpin->setValue(axisLabelUp_[1].y);
	ui.ViewYAxisUpZSpin->setValue(axisLabelUp_[1].z);
	ui.ViewYAxisRotationSpin->setValue(axisLabelRotation_.y);
	// -- Z
	ui.ViewZAxisDirectionXSpin->setValue(axisLabelDirection_[2].x);
	ui.ViewZAxisDirectionYSpin->setValue(axisLabelDirection_[2].y);
	ui.ViewZAxisDirectionZSpin->setValue(axisLabelDirection_[2].z);
	ui.ViewZAxisUpXSpin->setValue(axisLabelUp_[2].x);
	ui.ViewZAxisUpYSpin->setValue(axisLabelUp_[2].y);
	ui.ViewZAxisUpZSpin->setValue(axisLabelUp_[2].z);
	ui.ViewZAxisRotationSpin->setValue(axisLabelRotation_.z);

	// Extras
	if (boundingBox_ == UChromaWindow::NoBox) ui.ViewBoundingBoxNoneRadio->setChecked(true);
	else if (boundingBox_ == UChromaWindow::PlaneBox) ui.ViewBoundingBoxPlaneRadio->setChecked(true);
	else if (boundingBox_ == UChromaWindow::CubeBox) ui.ViewBoundingBoxCubeRadio->setChecked(true);
	ui.ViewBoundingBoxPlaneYSpin->setValue(boundingBoxPlaneY_);

	refreshing_ = false;
}
