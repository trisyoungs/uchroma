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
	axisInverted_[axis] = checked;
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

bool UChromaWindow::viewAxisTickOrientationChanged(int axis, int dir, double value)
{
	if (refreshing_) return false;
	axisTickDirection_[axis].set(dir, value);
	setAsModified();
	updateSurface(false);
	return true;
}

bool UChromaWindow::viewAxisLabelOrientationChanged(int axis, int component, double value)
{
	if (refreshing_) return false;
	axisLabelOrientation_[axis].set(component, value);
	setAsModified();
	updateSurface(false);
	return true;
}

bool UChromaWindow::viewAxisTitleOrientationChanged(int axis, int component, double value)
{
	if (refreshing_) return false;
	axisTitleOrientation_[axis].set(component, value);
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

bool UChromaWindow::viewAxisTitleChanged(int axis, QString& title)
{
	if (refreshing_) return false;
	axisTitle_[axis] = title;
	setAsModified();
	updateSurface(false);
	return true;
}

bool UChromaWindow::viewAxisTitleAlignmentChanged(int axis, int anchor)
{
	if (refreshing_) return false;
	axisTitleAnchor_[axis] = anchor;
	setAsModified();
	updateSurface(false);
	return true;
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

void UChromaWindow::on_ViewXAxisTitleEdit_textChanged(QString text)
{
	viewAxisTitleChanged(0, text);
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

void UChromaWindow::on_ViewXAxisTickDirectionXSpin_valueChanged(double value)
{
	viewAxisTickOrientationChanged(0, 0, value);
}

void UChromaWindow::on_ViewXAxisTickDirectionYSpin_valueChanged(double value)
{
	viewAxisTickOrientationChanged(0, 1, value);
}

void UChromaWindow::on_ViewXAxisTickDirectionZSpin_valueChanged(double value)
{
	viewAxisTickOrientationChanged(0, 2, value);
}

void UChromaWindow::on_ViewXAxisLabelAxialRotationSlider_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(0, 0, value);
}

void UChromaWindow::on_ViewXAxisLabelAxialRotationSpin_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(0, 0, value);
}

void UChromaWindow::on_ViewXAxisLabelInPlaneRotationSlider_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(0, 1, value);
}

void UChromaWindow::on_ViewXAxisLabelInPlaneRotationSpin_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(0, 1, value);
}

void UChromaWindow::on_ViewXAxisLabelDistanceSpin_valueChanged(double value)
{
	viewAxisLabelOrientationChanged(0, 2, value);
}

void UChromaWindow::on_ViewXAxisTitleHOffsetSlider_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(0, 3, double(value) / 1000.0);
}

void UChromaWindow::on_ViewXAxisTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.ViewXAxisTitleHOffsetSlider->setValue(0);
	viewAxisTitleOrientationChanged(0, 3, 0.0);
}

void UChromaWindow::on_ViewXAxisTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.ViewXAxisTitleHOffsetSlider->setValue(500);
	viewAxisTitleOrientationChanged(0, 3, 0.5);
}

void UChromaWindow::on_ViewXAxisTitleHOffsetRightButton_clicked(bool checked)
{
	ui.ViewXAxisTitleHOffsetSlider->setValue(1000);
	viewAxisTitleOrientationChanged(0, 3, 1.0);
}

void UChromaWindow::on_ViewXAxisTitleAnchorCombo_currentIndexChanged(int index)
{
	viewAxisTitleAlignmentChanged(0, index);
}

void UChromaWindow::on_ViewXAxisTitleAxialRotationSlider_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(0, 0, value);
}

void UChromaWindow::on_ViewXAxisTitleAxialRotationSpin_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(0, 0, value);
}

void UChromaWindow::on_ViewXAxisTitleInPlaneRotationSlider_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(0, 1, value);
}

void UChromaWindow::on_ViewXAxisTitleInPlaneRotationSpin_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(0, 1, value);
}

void UChromaWindow::on_ViewXAxisTitleDistanceSpin_valueChanged(double value)
{
	viewAxisTitleOrientationChanged(0, 2, value);
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

void UChromaWindow::on_ViewYAxisTitleEdit_textChanged(QString text)
{
	viewAxisTitleChanged(0, text);
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

void UChromaWindow::on_ViewYAxisTickDirectionXSpin_valueChanged(double value)
{
	viewAxisTickOrientationChanged(1, 0, value);
}

void UChromaWindow::on_ViewYAxisTickDirectionYSpin_valueChanged(double value)
{
	viewAxisTickOrientationChanged(1, 1, value);
}

void UChromaWindow::on_ViewYAxisTickDirectionZSpin_valueChanged(double value)
{
	viewAxisTickOrientationChanged(1, 2, value);
}

void UChromaWindow::on_ViewYAxisLabelAxialRotationSlider_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(1, 0, value);
}

void UChromaWindow::on_ViewYAxisLabelAxialRotationSpin_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(1, 0, value);
}

void UChromaWindow::on_ViewYAxisLabelInPlaneRotationSlider_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(1, 1, value);
}

void UChromaWindow::on_ViewYAxisLabelInPlaneRotationSpin_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(1, 1, value);
}

void UChromaWindow::on_ViewYAxisLabelDistanceSpin_valueChanged(double value)
{
	viewAxisLabelOrientationChanged(1, 2, value);
}

void UChromaWindow::on_ViewYAxisTitleHOffsetSlider_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(1, 3, double(value) / 1000.0);
}

void UChromaWindow::on_ViewYAxisTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.ViewYAxisTitleHOffsetSlider->setValue(0);
	viewAxisTitleOrientationChanged(1, 3, 0.0);
}

void UChromaWindow::on_ViewYAxisTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.ViewYAxisTitleHOffsetSlider->setValue(500);
	viewAxisTitleOrientationChanged(1, 3, 0.5);
}

void UChromaWindow::on_ViewYAxisTitleHOffsetRightButton_clicked(bool checked)
{
	ui.ViewYAxisTitleHOffsetSlider->setValue(1000);
	viewAxisTitleOrientationChanged(1, 3, 1.0);
}

void UChromaWindow::on_ViewYAxisTitleAnchorCombo_currentIndexChanged(int index)
{
	viewAxisTitleAlignmentChanged(0, index);
}

void UChromaWindow::on_ViewYAxisTitleAxialRotationSlider_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(1, 0, value);
}

void UChromaWindow::on_ViewYAxisTitleAxialRotationSpin_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(1, 0, value);
}

void UChromaWindow::on_ViewYAxisTitleInPlaneRotationSlider_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(1, 1, value);
}

void UChromaWindow::on_ViewYAxisTitleInPlaneRotationSpin_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(1, 1, value);
}

void UChromaWindow::on_ViewYAxisTitleDistanceSpin_valueChanged(double value)
{
	viewAxisTitleOrientationChanged(1, 2, value);
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

void UChromaWindow::on_ViewZAxisTitleEdit_textChanged(QString text)
{
	viewAxisTitleChanged(0, text);
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

void UChromaWindow::on_ViewZAxisTickDirectionXSpin_valueChanged(double value)
{
	viewAxisTickOrientationChanged(2, 0, value);
}

void UChromaWindow::on_ViewZAxisTickDirectionYSpin_valueChanged(double value)
{
	viewAxisTickOrientationChanged(2, 1, value);
}

void UChromaWindow::on_ViewZAxisTickDirectionZSpin_valueChanged(double value)
{
	viewAxisTickOrientationChanged(2, 2, value);
}

void UChromaWindow::on_ViewZAxisLabelAxialRotationSlider_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(2, 0, value);
}

void UChromaWindow::on_ViewZAxisLabelAxialRotationSpin_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(2, 0, value);
}

void UChromaWindow::on_ViewZAxisLabelInPlaneRotationSlider_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(2, 1, value);
}

void UChromaWindow::on_ViewZAxisLabelInPlaneRotationSpin_valueChanged(int value)
{
	viewAxisLabelOrientationChanged(2, 1, value);
}

void UChromaWindow::on_ViewZAxisLabelDistanceSpin_valueChanged(double value)
{
	viewAxisLabelOrientationChanged(2, 2, value);
}

void UChromaWindow::on_ViewZAxisTitleHOffsetSlider_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(2, 3, double(value) / 1000.0);
}

void UChromaWindow::on_ViewZAxisTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.ViewZAxisTitleHOffsetSlider->setValue(0);
	viewAxisTitleOrientationChanged(2, 3, 0.0);
}

void UChromaWindow::on_ViewZAxisTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.ViewZAxisTitleHOffsetSlider->setValue(500);
	viewAxisTitleOrientationChanged(2, 3, 0.5);
}

void UChromaWindow::on_ViewZAxisTitleHOffsetRightButton_clicked(bool checked)
{
	ui.ViewZAxisTitleHOffsetSlider->setValue(1000);
	viewAxisTitleOrientationChanged(2, 3, 1.0);
}

void UChromaWindow::on_ViewZAxisTitleAnchorCombo_currentIndexChanged(int index)
{
	viewAxisTitleAlignmentChanged(2, index);
}

void UChromaWindow::on_ViewZAxisTitleAxialRotationSlider_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(2, 0, value);
}

void UChromaWindow::on_ViewZAxisTitleAxialRotationSpin_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(2, 0, value);
}

void UChromaWindow::on_ViewZAxisTitleInPlaneRotationSlider_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(2, 1, value);
}

void UChromaWindow::on_ViewZAxisTitleInPlaneRotationSpin_valueChanged(int value)
{
	viewAxisTitleOrientationChanged(2, 1, value);
}

void UChromaWindow::on_ViewZAxisTitleDistanceSpin_valueChanged(double value)
{
	viewAxisTitleOrientationChanged(2, 2, value);
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

// -- General Options
void UChromaWindow::on_ViewLabelsFaceViewerCheck_clicked(bool checked)
{
	if (refreshing_) return;
	labelFaceViewer_ = checked;
	setAsModified();
	updateSurface(false);
}

void UChromaWindow::on_ViewLabelScaleSpin_valueChanged(double value)
{
	if (refreshing_) return;
	labelScale_ = value;
	setAsModified();
	updateSurface(false);
}

void UChromaWindow::on_ViewTitleScaleSpin_valueChanged(double value)
{
	if (refreshing_) return;
	titleScale_ = value;
	setAsModified();
	updateSurface(false);
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
	ui.ViewXAxisInvertCheck->setChecked(axisInverted_.x);
	ui.ViewYAxisInvertCheck->setChecked(axisInverted_.y);
	ui.ViewZAxisInvertCheck->setChecked(axisInverted_.z);
	ui.ViewXAxisVisibleCheck->setChecked(axisVisible_.x);
	ui.ViewYAxisVisibleCheck->setChecked(axisVisible_.y);
	ui.ViewZAxisVisibleCheck->setChecked(axisVisible_.z);
	ui.ViewXAxisLogarithmicCheck->setChecked(axisLogarithmic_.x);
	ui.ViewYAxisLogarithmicCheck->setChecked(axisLogarithmic_.y);
	ui.ViewZAxisLogarithmicCheck->setChecked(axisLogarithmic_.z);

	// Axis titles
	ui.ViewXAxisTitleEdit->setText(axisTitle_[0]);
	ui.ViewYAxisTitleEdit->setText(axisTitle_[1]);
	ui.ViewZAxisTitleEdit->setText(axisTitle_[2]);

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

	// Axis Ticks
	// -- X
	ui.ViewXAxisAutoTicksCheck->setChecked(axisAutoTicks_.x);
	ui.ViewXAxisTicksStartSpin->setValue(axisFirstTick_.x);
	ui.ViewXAxisTicksStartSpin->setEnabled(!axisAutoTicks_.x);
	ui.ViewXAxisTicksDeltaSpin->setValue(axisTickDelta_.x);
	ui.ViewXAxisTicksDeltaSpin->setEnabled(!axisAutoTicks_.x);
	ui.ViewXAxisMinorTicksSpin->setValue(axisMinorTicks_.x);
	ui.ViewXAxisTickDirectionXSpin->setValue(axisTickDirection_[0].x);
	ui.ViewXAxisTickDirectionYSpin->setValue(axisTickDirection_[0].y);
	ui.ViewXAxisTickDirectionZSpin->setValue(axisTickDirection_[0].z);
	// -- Y
	ui.ViewYAxisAutoTicksCheck->setChecked(axisAutoTicks_.y);
	ui.ViewYAxisTicksStartSpin->setValue(axisFirstTick_.y);
	ui.ViewYAxisTicksStartSpin->setEnabled(!axisAutoTicks_.y);
	ui.ViewYAxisTicksDeltaSpin->setValue(axisTickDelta_.y);
	ui.ViewYAxisTicksDeltaSpin->setEnabled(!axisAutoTicks_.y);
	ui.ViewYAxisMinorTicksSpin->setValue(axisMinorTicks_.y);
	ui.ViewYAxisTickDirectionXSpin->setValue(axisTickDirection_[1].x);
	ui.ViewYAxisTickDirectionYSpin->setValue(axisTickDirection_[1].y);
	ui.ViewYAxisTickDirectionZSpin->setValue(axisTickDirection_[1].z);
	// -- Z
	ui.ViewZAxisAutoTicksCheck->setChecked(axisAutoTicks_.z);
	ui.ViewZAxisTicksStartSpin->setValue(axisFirstTick_.z);
	ui.ViewZAxisTicksStartSpin->setEnabled(!axisAutoTicks_.z);
	ui.ViewZAxisTicksDeltaSpin->setValue(axisTickDelta_.z);
	ui.ViewZAxisTicksDeltaSpin->setEnabled(!axisAutoTicks_.z);
	ui.ViewZAxisMinorTicksSpin->setValue(axisMinorTicks_.z);
	ui.ViewZAxisTickDirectionXSpin->setValue(axisTickDirection_[2].x);
	ui.ViewZAxisTickDirectionYSpin->setValue(axisTickDirection_[2].y);
	ui.ViewZAxisTickDirectionZSpin->setValue(axisTickDirection_[2].z);

	// Text Orientation
	// -- X
	ui.ViewXAxisLabelAxialRotationSlider->setValue(axisLabelOrientation_[0].x);
	ui.ViewXAxisLabelAxialRotationSpin->setValue(axisLabelOrientation_[0].x);
	ui.ViewXAxisLabelInPlaneRotationSlider->setValue(axisLabelOrientation_[0].y);
	ui.ViewXAxisLabelInPlaneRotationSpin->setValue(axisLabelOrientation_[0].y);
	ui.ViewXAxisLabelDistanceSpin->setValue(axisLabelOrientation_[0].z);
	ui.ViewYAxisTitleHOffsetSlider->setValue(axisTitleOrientation_[0].w*1000);
	ui.ViewXAxisTitleAnchorCombo->setCurrentIndex(axisTitleAnchor_.x);
	ui.ViewXAxisTitleAxialRotationSlider->setValue(axisTitleOrientation_[0].x);
	ui.ViewXAxisTitleAxialRotationSpin->setValue(axisTitleOrientation_[0].x);
	ui.ViewXAxisTitleInPlaneRotationSlider->setValue(axisTitleOrientation_[0].y);
	ui.ViewXAxisTitleInPlaneRotationSpin->setValue(axisTitleOrientation_[0].y);
	ui.ViewXAxisTitleDistanceSpin->setValue(axisTitleOrientation_[0].z);
	// -- Y
	ui.ViewYAxisLabelAxialRotationSlider->setValue(axisLabelOrientation_[1].x);
	ui.ViewYAxisLabelAxialRotationSpin->setValue(axisLabelOrientation_[1].x);
	ui.ViewYAxisLabelInPlaneRotationSlider->setValue(axisLabelOrientation_[1].y);
	ui.ViewYAxisLabelInPlaneRotationSpin->setValue(axisLabelOrientation_[1].y);
	ui.ViewYAxisLabelDistanceSpin->setValue(axisLabelOrientation_[1].z);
	ui.ViewYAxisTitleHOffsetSlider->setValue(axisTitleOrientation_[1].w*1000);
	ui.ViewYAxisTitleAnchorCombo->setCurrentIndex(axisTitleAnchor_.y);
	ui.ViewYAxisTitleAxialRotationSlider->setValue(axisTitleOrientation_[1].x);
	ui.ViewYAxisTitleAxialRotationSpin->setValue(axisTitleOrientation_[1].x);
	ui.ViewYAxisTitleInPlaneRotationSlider->setValue(axisTitleOrientation_[1].y);
	ui.ViewYAxisTitleInPlaneRotationSpin->setValue(axisTitleOrientation_[1].y);
	ui.ViewYAxisTitleDistanceSpin->setValue(axisTitleOrientation_[1].z);
	// -- Z
	ui.ViewZAxisLabelAxialRotationSlider->setValue(axisLabelOrientation_[2].x);
	ui.ViewZAxisLabelAxialRotationSpin->setValue(axisLabelOrientation_[2].x);
	ui.ViewZAxisLabelInPlaneRotationSlider->setValue(axisLabelOrientation_[2].y);
	ui.ViewZAxisLabelInPlaneRotationSpin->setValue(axisLabelOrientation_[2].y);
	ui.ViewZAxisLabelDistanceSpin->setValue(axisLabelOrientation_[2].z);
	ui.ViewZAxisTitleHOffsetSlider->setValue(axisTitleOrientation_[2].w*1000);
	ui.ViewZAxisTitleAnchorCombo->setCurrentIndex(axisTitleAnchor_.z);
	ui.ViewZAxisTitleAxialRotationSlider->setValue(axisTitleOrientation_[2].x);
	ui.ViewZAxisTitleAxialRotationSpin->setValue(axisTitleOrientation_[2].x);
	ui.ViewZAxisTitleInPlaneRotationSlider->setValue(axisTitleOrientation_[2].y);
	ui.ViewZAxisTitleInPlaneRotationSpin->setValue(axisTitleOrientation_[2].y);
	ui.ViewZAxisTitleDistanceSpin->setValue(axisTitleOrientation_[2].z);
	
	// Extras
	if (boundingBox_ == UChromaWindow::NoBox) ui.ViewBoundingBoxNoneRadio->setChecked(true);
	else if (boundingBox_ == UChromaWindow::PlaneBox) ui.ViewBoundingBoxPlaneRadio->setChecked(true);
	else if (boundingBox_ == UChromaWindow::CubeBox) ui.ViewBoundingBoxCubeRadio->setChecked(true);
	ui.ViewBoundingBoxPlaneYSpin->setValue(boundingBoxPlaneY_);

	// General Options
	ui.ViewLabelsFaceViewerCheck->setChecked(labelFaceViewer_);

	refreshing_ = false;
}
