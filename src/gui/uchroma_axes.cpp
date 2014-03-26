/*
	*** Main Window - Axes Functions 
	*** src/gui/uchroma_axes.cpp
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
#include <limits>

bool UChromaWindow::axisInvertChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	axisInverted_[axis] = checked;

	// Update relevant parts of gui, the axes primitives, flag to regenerate collection's surface data, and finally the display
	setAsModified();
	updateAxes();
	regenerateAll();
	updateDisplay();

	return true;
}

bool UChromaWindow::axisLogarithmicChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	axisLogarithmic_[axis] = checked;

	// Update relevant parts of gui, the axes primitives, flag to regenerate collection's surface data, and finally the display
	setAsModified();
	updateAxes();
	regenerateAll();
	updateDisplay();

	return true;
}

bool UChromaWindow::axisVisibleChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	axisVisible_[axis] = checked;

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxes();
	updateDisplay();

	return true;
}

bool UChromaWindow::axisStretchChanged(int axis, double value)
{
	if (refreshing_) return false;
	axisStretch_[axis] = value;

	// Update relevant parts of gui, the axes primitives, flag to regenerate collection's surface data, and finally the display
	setAsModified();
	updateAxes();
	regenerateAll();
	updateDisplay();

	return true;
}

bool UChromaWindow::axisLimitChanged(int axis, bool minLim, double value)
{
	if (refreshing_) return false;
	if (minLim) axisMin_[axis] = value;
	else axisMax_[axis] = value;

	// Update relevant parts of gui, the axes primitives, flag to regenerate collection's surface data, and finally the display
	setAsModified();
	updateAxesTab();
	updateAxes();
	regenerateAll();
	updateDisplay();

	return true;
}

bool UChromaWindow::axisLimitSetExtreme(int axis, bool minLim)
{
	if (refreshing_) return false;
	if (minLim) axisMin_[axis] = transformedDataMinima()[axis];
	else axisMax_[axis] = transformedDataMaxima()[axis];

	// Update relevant parts of gui, the axes primitives, flag to regenerate collection's surface data, and finally the display
	setAsModified();
	updateAxesTab();
	updateAxes();
	regenerateAll();
	updateDisplay();

	return true;
}

bool UChromaWindow::axisCrossChanged(int axis, int dir, double value)
{
	if (refreshing_) return false;
	axisPosition_[axis].set(dir,value);

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxes();
	updateDisplay();

	return true;
}

bool UChromaWindow::axisCrossSet(int axis, int dir, int type)
{
	if (refreshing_) return false;
	if (type == -1) axisPosition_[axis].set(dir, axisMin_[dir]);
	else if ((type == 0) && (axisMin_[dir] <= 0.0) && (axisMax_[dir] >= 0.0)) axisPosition_[axis].set(dir, 0.0);
	else if (type == 1) axisPosition_[axis].set(dir, axisMax_[dir]);
	else return false;

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxesTab();
	updateAxes();
	updateDisplay();
	
	return true;
}

bool UChromaWindow::axisAutoTicksChanged(int axis, bool enabled)
{
	if (refreshing_) return false;
	axisAutoTicks_[axis] = enabled;

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxesTab();
	updateAxes();
	updateDisplay();
	
	return true;
}

bool UChromaWindow::axisTicksChanged(int axis, bool start, double value)
{
	if (refreshing_) return false;
	if (start) axisFirstTick_[axis] = value;
	else axisTickDelta_[axis] = value;

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxes();
	updateDisplay();
	
	return true;
}

bool UChromaWindow::axisTickOrientationChanged(int axis, int dir, double value)
{
	if (refreshing_) return false;
	axisTickDirection_[axis].set(dir, value);

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxes();
	updateDisplay();
	
	return true;
}

bool UChromaWindow::axisLabelOrientationChanged(int axis, int component, double value)
{
	if (refreshing_) return false;
	axisLabelOrientation_[axis].set(component, value);

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxes();
	updateDisplay();
	
	return true;
}

bool UChromaWindow::axisTitleOrientationChanged(int axis, int component, double value)
{
	if (refreshing_) return false;
	axisTitleOrientation_[axis].set(component, value);

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxes();
	updateDisplay();
	
	return true;
}

bool UChromaWindow::axisMinorTicksChanged(int axis, int value)
{
	if (refreshing_) return false;
	axisMinorTicks_[axis] = value;

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxes();
	updateDisplay();
	
	return true;
}

bool UChromaWindow::axisTitleChanged(int axis, QString& title)
{
	if (refreshing_) return false;
	axisTitle_[axis] = title;

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxes();
	updateDisplay();
	updateSlicesTab();
	updateSliceMonitor();
	
	return true;
}

bool UChromaWindow::axisTitleAlignmentChanged(int axis, TextPrimitive::HorizontalAnchor anchor)
{
	if (refreshing_) return false;
	axisTitleAnchor_[axis] = anchor;

	// Update relevant parts of gui, the axes primitives, and finally the display
	setAsModified();
	updateAxes();
	updateDisplay();
	
	return true;
}

// X Axis

void UChromaWindow::on_AxisXInvertCheck_clicked(bool checked)
{
	axisInvertChanged(0, checked);
}

void UChromaWindow::on_AxisXLogarithmicCheck_clicked(bool checked)
{
	axisLogarithmicChanged(0, checked);
}

void UChromaWindow::on_AxisXVisibleCheck_clicked(bool checked)
{
	axisVisibleChanged(0, checked);
}

void UChromaWindow::on_AxisXTitleEdit_textChanged(QString text)
{
	axisTitleChanged(0, text);
}

void UChromaWindow::on_AxisXStretchSpin_valueChanged(double value)
{
	axisStretchChanged(0, value);
}

void UChromaWindow::on_AxisXMinSpin_valueChanged(double value)
{
	axisLimitChanged(0, true, value);
}

void UChromaWindow::on_AxisYMinSpin_valueChanged(double value)
{
	axisLimitChanged(1, true, value);
}

void UChromaWindow::on_AxisZMinSpin_valueChanged(double value)
{
	axisLimitChanged(2, true, value);
}

void UChromaWindow::on_AxisXMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(0, true);
}

void UChromaWindow::on_AxisYMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(1, true);
}

void UChromaWindow::on_AxisZMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(2, true);
}

void UChromaWindow::on_AxisXMaxSpin_valueChanged(double value)
{
	axisLimitChanged(0, false, value);
}

void UChromaWindow::on_AxisYMaxSpin_valueChanged(double value)
{
	axisLimitChanged(1, false, value);
}

void UChromaWindow::on_AxisZMaxSpin_valueChanged(double value)
{
	axisLimitChanged(2, false, value);
}

void UChromaWindow::on_AxisXMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(0, false);
}

void UChromaWindow::on_AxisYMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(1, false);
}

void UChromaWindow::on_AxisZMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(2, false);
}

void UChromaWindow::on_AxisXCrossAtYSpin_valueChanged(double value)
{
	axisCrossChanged(0, 1, value);
}

void UChromaWindow::on_AxisXCrossAtZSpin_valueChanged(double value)
{
	axisCrossChanged(0, 2, value);
}

void UChromaWindow::on_AxisXCrossAtYSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(0, 1, -1);
}

void UChromaWindow::on_AxisXCrossAtYSetZeroButton_clicked(bool checked)
{
	axisCrossSet(0, 1, 0);
}

void UChromaWindow::on_AxisXCrossAtYSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(0, 1, 1);
}

void UChromaWindow::on_AxisXCrossAtZSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(0, 2, -1);
}

void UChromaWindow::on_AxisXCrossAtZSetZeroButton_clicked(bool checked)
{
	axisCrossSet(0, 2, 0);
}

void UChromaWindow::on_AxisXCrossAtZSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(0, 2, 1);
}

void UChromaWindow::on_AxisXAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicksChanged(0, checked);
}

void UChromaWindow::on_AxisXTicksStartSpin_valueChanged(double value)
{
	axisTicksChanged(0, true, value);
}

void UChromaWindow::on_AxisXTicksDeltaSpin_valueChanged(double value)
{
	axisTicksChanged(0, false, value);
}

void UChromaWindow::on_AxisXMinorTicksSpin_valueChanged(int value)
{
	axisMinorTicksChanged(0, value);
}

void UChromaWindow::on_AxisXTickDirectionXSpin_valueChanged(double value)
{
	axisTickOrientationChanged(0, 0, value);
}

void UChromaWindow::on_AxisXTickDirectionYSpin_valueChanged(double value)
{
	axisTickOrientationChanged(0, 1, value);
}

void UChromaWindow::on_AxisXTickDirectionZSpin_valueChanged(double value)
{
	axisTickOrientationChanged(0, 2, value);
}

void UChromaWindow::on_AxisXLabelAxialRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(0, 0, value);
}

void UChromaWindow::on_AxisXLabelAxialRotationSpin_valueChanged(int value)
{
	axisLabelOrientationChanged(0, 0, value);
}

void UChromaWindow::on_AxisXLabelInPlaneRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(0, 1, value);
}

void UChromaWindow::on_AxisXLabelInPlaneRotationSpin_valueChanged(int value)
{
	axisLabelOrientationChanged(0, 1, value);
}

void UChromaWindow::on_AxisXLabelDistanceSpin_valueChanged(double value)
{
	axisLabelOrientationChanged(0, 2, value);
}

void UChromaWindow::on_AxisXTitleHOffsetSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 3, double(value) / 1000.0);
}

void UChromaWindow::on_AxisXTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.AxisXTitleHOffsetSlider->setValue(0);
	axisTitleOrientationChanged(0, 3, 0.0);
}

void UChromaWindow::on_AxisXTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.AxisXTitleHOffsetSlider->setValue(500);
	axisTitleOrientationChanged(0, 3, 0.5);
}

void UChromaWindow::on_AxisXTitleHOffsetRightButton_clicked(bool checked)
{
	ui.AxisXTitleHOffsetSlider->setValue(1000);
	axisTitleOrientationChanged(0, 3, 1.0);
}

void UChromaWindow::on_AxisXTitleAnchorCombo_currentIndexChanged(int index)
{
	axisTitleAlignmentChanged(0, (TextPrimitive::HorizontalAnchor) index);
}

void UChromaWindow::on_AxisXTitleAxialRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 0, value);
}

void UChromaWindow::on_AxisXTitleAxialRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 0, value);
}

void UChromaWindow::on_AxisXTitleInPlaneRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 1, value);
}

void UChromaWindow::on_AxisXTitleInPlaneRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 1, value);
}

void UChromaWindow::on_AxisXTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(0, 2, value);
}

// Y Axis

void UChromaWindow::on_AxisYInvertCheck_clicked(bool checked)
{
	axisInvertChanged(1, checked);
}

void UChromaWindow::on_AxisYLogarithmicCheck_clicked(bool checked)
{
	axisLogarithmicChanged(1, checked);
}

void UChromaWindow::on_AxisYVisibleCheck_clicked(bool checked)
{
	axisVisibleChanged(1, checked);
}

void UChromaWindow::on_AxisYTitleEdit_textChanged(QString text)
{
	axisTitleChanged(1, text);
}

void UChromaWindow::on_AxisYStretchSpin_valueChanged(double value)
{
	axisStretchChanged(1, value);
}

void UChromaWindow::on_AxisYCrossAtXSpin_valueChanged(double value)
{
	axisCrossChanged(1, 0, value);
}

void UChromaWindow::on_AxisYCrossAtZSpin_valueChanged(double value)
{
	axisCrossChanged(1, 2, value);
}

void UChromaWindow::on_AxisYCrossAtXSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(1, 0, -1);
}

void UChromaWindow::on_AxisYCrossAtXSetZeroButton_clicked(bool checked)
{
	axisCrossSet(1, 0, 0);
}

void UChromaWindow::on_AxisYCrossAtXSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(1, 0, 1);
}

void UChromaWindow::on_AxisYCrossAtZSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(1, 2, -1);
}

void UChromaWindow::on_AxisYCrossAtZSetZeroButton_clicked(bool checked)
{
	axisCrossSet(1, 2, 0);
}

void UChromaWindow::on_AxisYCrossAtZSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(1, 2, 1);
}

void UChromaWindow::on_AxisYAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicksChanged(1, checked);
}

void UChromaWindow::on_AxisYTicksStartSpin_valueChanged(double value)
{
	axisTicksChanged(1, true, value);
}

void UChromaWindow::on_AxisYTicksDeltaSpin_valueChanged(double value)
{
	axisTicksChanged(1, false, value);
}

void UChromaWindow::on_AxisYMinorTicksSpin_valueChanged(int value)
{
	axisMinorTicksChanged(1, value);
}

void UChromaWindow::on_AxisYTickDirectionXSpin_valueChanged(double value)
{
	axisTickOrientationChanged(1, 0, value);
}

void UChromaWindow::on_AxisYTickDirectionYSpin_valueChanged(double value)
{
	axisTickOrientationChanged(1, 1, value);
}

void UChromaWindow::on_AxisYTickDirectionZSpin_valueChanged(double value)
{
	axisTickOrientationChanged(1, 2, value);
}

void UChromaWindow::on_AxisYLabelAxialRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(1, 0, value);
}

void UChromaWindow::on_AxisYLabelAxialRotationSpin_valueChanged(int value)
{
	axisLabelOrientationChanged(1, 0, value);
}

void UChromaWindow::on_AxisYLabelInPlaneRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(1, 1, value);
}

void UChromaWindow::on_AxisYLabelInPlaneRotationSpin_valueChanged(int value)
{
	axisLabelOrientationChanged(1, 1, value);
}

void UChromaWindow::on_AxisYLabelDistanceSpin_valueChanged(double value)
{
	axisLabelOrientationChanged(1, 2, value);
}

void UChromaWindow::on_AxisYTitleHOffsetSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 3, double(value) / 1000.0);
}

void UChromaWindow::on_AxisYTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.AxisYTitleHOffsetSlider->setValue(0);
	axisTitleOrientationChanged(1, 3, 0.0);
}

void UChromaWindow::on_AxisYTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.AxisYTitleHOffsetSlider->setValue(500);
	axisTitleOrientationChanged(1, 3, 0.5);
}

void UChromaWindow::on_AxisYTitleHOffsetRightButton_clicked(bool checked)
{
	ui.AxisYTitleHOffsetSlider->setValue(1000);
	axisTitleOrientationChanged(1, 3, 1.0);
}

void UChromaWindow::on_AxisYTitleAnchorCombo_currentIndexChanged(int index)
{
	axisTitleAlignmentChanged(1, (TextPrimitive::HorizontalAnchor) index);
}

void UChromaWindow::on_AxisYTitleAxialRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 0, value);
}

void UChromaWindow::on_AxisYTitleAxialRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 0, value);
}

void UChromaWindow::on_AxisYTitleInPlaneRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 1, value);
}

void UChromaWindow::on_AxisYTitleInPlaneRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 1, value);
}

void UChromaWindow::on_AxisYTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(1, 2, value);
}

// Z Axis

void UChromaWindow::on_AxisZInvertCheck_clicked(bool checked)
{
	axisInvertChanged(2, checked);
}

void UChromaWindow::on_AxisZLogarithmicCheck_clicked(bool checked)
{
	axisLogarithmicChanged(2, checked);
}

void UChromaWindow::on_AxisZVisibleCheck_clicked(bool checked)
{
	axisVisibleChanged(2, checked);
}

void UChromaWindow::on_AxisZTitleEdit_textChanged(QString text)
{
	axisTitleChanged(2, text);
}

void UChromaWindow::on_AxisZStretchSpin_valueChanged(double value)
{
	axisStretchChanged(2, value);
}

void UChromaWindow::on_AxisZCrossAtXSpin_valueChanged(double value)
{
	axisCrossChanged(2, 0, value);
}

void UChromaWindow::on_AxisZCrossAtYSpin_valueChanged(double value)
{
	axisCrossChanged(2, 1, value);
}

void UChromaWindow::on_AxisZCrossAtXSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(2, 0, -1);
}

void UChromaWindow::on_AxisZCrossAtXSetZeroButton_clicked(bool checked)
{
	axisCrossSet(2, 0, 0);
}

void UChromaWindow::on_AxisZCrossAtXSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(2, 0, 1);
}

void UChromaWindow::on_AxisZCrossAtYSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(2, 1, -1);
}

void UChromaWindow::on_AxisZCrossAtYSetZeroButton_clicked(bool checked)
{
	axisCrossSet(2, 1, 0);
}

void UChromaWindow::on_AxisZCrossAtYSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(2, 1, 1);
}

void UChromaWindow::on_AxisZAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicksChanged(2, checked);
}

void UChromaWindow::on_AxisZTicksStartSpin_valueChanged(double value)
{
	axisTicksChanged(2, true, value);
}

void UChromaWindow::on_AxisZTicksDeltaSpin_valueChanged(double value)
{
	axisTicksChanged(2, false, value);
}

void UChromaWindow::on_AxisZMinorTicksSpin_valueChanged(int value)
{
	axisMinorTicksChanged(2, value);
}

void UChromaWindow::on_AxisZTickDirectionXSpin_valueChanged(double value)
{
	axisTickOrientationChanged(2, 0, value);
}

void UChromaWindow::on_AxisZTickDirectionYSpin_valueChanged(double value)
{
	axisTickOrientationChanged(2, 1, value);
}

void UChromaWindow::on_AxisZTickDirectionZSpin_valueChanged(double value)
{
	axisTickOrientationChanged(2, 2, value);
}

void UChromaWindow::on_AxisZLabelAxialRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(2, 0, value);
}

void UChromaWindow::on_AxisZLabelAxialRotationSpin_valueChanged(int value)
{
	axisLabelOrientationChanged(2, 0, value);
}

void UChromaWindow::on_AxisZLabelInPlaneRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(2, 1, value);
}

void UChromaWindow::on_AxisZLabelInPlaneRotationSpin_valueChanged(int value)
{
	axisLabelOrientationChanged(2, 1, value);
}

void UChromaWindow::on_AxisZLabelDistanceSpin_valueChanged(double value)
{
	axisLabelOrientationChanged(2, 2, value);
}

void UChromaWindow::on_AxisZTitleHOffsetSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 3, double(value) / 1000.0);
}

void UChromaWindow::on_AxisZTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.AxisZTitleHOffsetSlider->setValue(0);
	axisTitleOrientationChanged(2, 3, 0.0);
}

void UChromaWindow::on_AxisZTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.AxisZTitleHOffsetSlider->setValue(500);
	axisTitleOrientationChanged(2, 3, 0.5);
}

void UChromaWindow::on_AxisZTitleHOffsetRightButton_clicked(bool checked)
{
	ui.AxisZTitleHOffsetSlider->setValue(1000);
	axisTitleOrientationChanged(2, 3, 1.0);
}

void UChromaWindow::on_AxisZTitleAnchorCombo_currentIndexChanged(int index)
{
	axisTitleAlignmentChanged(2, (TextPrimitive::HorizontalAnchor) index);
}

void UChromaWindow::on_AxisZTitleAxialRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 0, value);
}

void UChromaWindow::on_AxisZTitleAxialRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 0, value);
}

void UChromaWindow::on_AxisZTitleInPlaneRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 1, value);
}

void UChromaWindow::on_AxisZTitleInPlaneRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 1, value);
}

void UChromaWindow::on_AxisZTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(2, 2, value);
}

/*
 * Extras Tab
 */

void UChromaWindow::on_ViewBoundingBoxNoneRadio_clicked(bool checked)
{
	if (refreshing_) return;
	boundingBox_ = UChromaWindow::NoBox;
	updateAxes();
	updateDisplay();
}

void UChromaWindow::on_ViewBoundingBoxPlaneRadio_clicked(bool checked)
{
	if (refreshing_) return;
	boundingBox_ = UChromaWindow::PlaneBox;
	updateAxes();
	updateDisplay();
}

void UChromaWindow::on_ViewBoundingBoxCubeRadio_clicked(bool checked)
{
	if (refreshing_) return;
	boundingBox_ = UChromaWindow::CubeBox;
	updateAxes();
	updateDisplay();
}

void UChromaWindow::on_ViewBoundingBoxPlaneYSpin_valueChanged(double value)
{
	if (refreshing_) return;
	boundingBoxPlaneY_ = value;
	updateAxes();
	updateDisplay();
}

void UChromaWindow::on_ViewBoundingBoxPlaneYSetMinimumButton_clicked(bool checked)
{
	ui.ViewBoundingBoxPlaneYSpin->setValue(axisMin_.y);
}

void UChromaWindow::on_ViewBoundingBoxPlaneYSetZeroButton_clicked(bool checked)
{
	ui.ViewBoundingBoxPlaneYSpin->setValue(0.0);
}

void UChromaWindow::on_ViewBoundingBoxPlaneYSetMaximumButton_clicked(bool checked)
{
	ui.ViewBoundingBoxPlaneYSpin->setValue(axisMax_.y);
}

// -- General Options
void UChromaWindow::on_ViewLabelsFaceViewerCheck_clicked(bool checked)
{
	if (refreshing_) return;
	labelFaceViewer_ = checked;
	setAsModified();
	updateAxes();
	updateDisplay();
}

void UChromaWindow::on_ViewLabelScaleSpin_valueChanged(double value)
{
	if (refreshing_) return;
	labelScale_ = value;
	setAsModified();
	updateAxes();
	updateDisplay();
}

void UChromaWindow::on_ViewTitleScaleSpin_valueChanged(double value)
{
	if (refreshing_) return;
	titleScale_ = value;
	setAsModified();
	updateAxes();
	updateDisplay();
}

/*
 * Update
 */
void UChromaWindow::updateAxesTab()
{
	refreshing_ = true;
	
	// Label scales
	ui.ViewLabelScaleSpin->setValue(labelScale_);
	ui.ViewTitleScaleSpin->setValue(titleScale_);

	// Invert / Visible / Logarithmic
	ui.AxisXInvertCheck->setChecked(axisInverted_.x);
	ui.AxisYInvertCheck->setChecked(axisInverted_.y);
	ui.AxisZInvertCheck->setChecked(axisInverted_.z);
	ui.AxisXVisibleCheck->setChecked(axisVisible_.x);
	ui.AxisYVisibleCheck->setChecked(axisVisible_.y);
	ui.AxisZVisibleCheck->setChecked(axisVisible_.z);
	ui.AxisXLogarithmicCheck->setChecked(axisLogarithmic_.x);
	ui.AxisYLogarithmicCheck->setChecked(axisLogarithmic_.y);
	ui.AxisZLogarithmicCheck->setChecked(axisLogarithmic_.z);
	ui.AxisXTicksStartDeltaAutoWidget->setDisabled(axisLogarithmic_.x);

	// Axis titles
	ui.AxisXTitleEdit->setText(axisTitle_[0]);
	ui.AxisYTitleEdit->setText(axisTitle_[1]);
	ui.AxisZTitleEdit->setText(axisTitle_[2]);

	// Axis Stretch factors
	ui.AxisXStretchSpin->setValue(axisStretch_.x);
	ui.AxisYStretchSpin->setValue(axisStretch_.y);
	ui.AxisZStretchSpin->setValue(axisStretch_.z);

	// Axis Min/Max Limits
	Vec3<double> transformMin = transformedDataMinima(), transformMax = transformedDataMaxima();
	ui.AxisXMinSpin->setRange(transformMin.x, transformMax.x);
	ui.AxisYMinSpin->setRange(transformMin.y, transformMax.y);
	ui.AxisZMinSpin->setRange(transformMin.z, transformMax.z);
	ui.AxisXMinSpin->setSingleStep(max((transformMax.x-transformMin.x)*0.01, 1.0));
	ui.AxisYMinSpin->setSingleStep(max((transformMax.y-transformMin.y)*0.01, 1.0));
	ui.AxisZMinSpin->setSingleStep(max((transformMax.z-transformMin.z)*0.01, 1.0));

	ui.AxisXMaxSpin->setRange(transformMin.x, transformMax.x);
	ui.AxisYMaxSpin->setRange(transformMin.y, transformMax.y);
	ui.AxisZMaxSpin->setRange(transformMin.z, transformMax.z);
	ui.AxisXMaxSpin->setSingleStep(max((transformMax.x-transformMin.x)*0.01, 1.0));
	ui.AxisYMaxSpin->setSingleStep(max((transformMax.y-transformMin.y)*0.01, 1.0));
	ui.AxisZMaxSpin->setSingleStep(max((transformMax.z-transformMin.z)*0.01, 1.0));
	ui.AxisXMinSpin->setValue(axisMin_.x);
	ui.AxisYMinSpin->setValue(axisMin_.y);
	ui.AxisZMinSpin->setValue(axisMin_.z);
	ui.AxisXMaxSpin->setValue(axisMax_.x);
	ui.AxisYMaxSpin->setValue(axisMax_.y);
	ui.AxisZMaxSpin->setValue(axisMax_.z);
	ui.AxisXMinLabel->setText("DataMin: " + QString::number(transformMin.x));
	ui.AxisXMaxLabel->setText("DataMax: " + QString::number(transformMax.x));
	ui.AxisYMinLabel->setText("DataMin: " + QString::number(transformMin.y));
	ui.AxisYMaxLabel->setText("DataMax: " + QString::number(transformMax.y));
	ui.AxisZMinLabel->setText("DataMin: " + QString::number(transformMin.z));
	ui.AxisZMaxLabel->setText("DataMax: " + QString::number(transformMax.z));

	// Axis positions
	// -- X
	ui.AxisXCrossAtYSpin->setValue(axisPosition_[0].y);
	ui.AxisXCrossAtZSpin->setValue(axisPosition_[0].z);
	// -- Y
	ui.AxisYCrossAtXSpin->setValue(axisPosition_[1].x);
	ui.AxisYCrossAtZSpin->setValue(axisPosition_[1].z);
	// -- Z
	ui.AxisZCrossAtXSpin->setValue(axisPosition_[2].x);
	ui.AxisZCrossAtYSpin->setValue(axisPosition_[2].y);

	// Axis Ticks
	// -- X
	ui.AxisXAutoTicksCheck->setChecked(axisAutoTicks_.x);
	ui.AxisXTicksStartSpin->setValue(axisFirstTick_.x);
	ui.AxisXTicksStartSpin->setEnabled(!axisAutoTicks_.x);
	ui.AxisXTicksDeltaSpin->setValue(axisTickDelta_.x);
	ui.AxisXTicksDeltaSpin->setEnabled(!axisAutoTicks_.x);
	ui.AxisXMinorTicksSpin->setValue(axisMinorTicks_.x);
	ui.AxisXTickDirectionXSpin->setValue(axisTickDirection_[0].x);
	ui.AxisXTickDirectionYSpin->setValue(axisTickDirection_[0].y);
	ui.AxisXTickDirectionZSpin->setValue(axisTickDirection_[0].z);
	// -- Y
	ui.AxisYAutoTicksCheck->setChecked(axisAutoTicks_.y);
	ui.AxisYTicksStartSpin->setValue(axisFirstTick_.y);
	ui.AxisYTicksStartSpin->setEnabled(!axisAutoTicks_.y);
	ui.AxisYTicksDeltaSpin->setValue(axisTickDelta_.y);
	ui.AxisYTicksDeltaSpin->setEnabled(!axisAutoTicks_.y);
	ui.AxisYMinorTicksSpin->setValue(axisMinorTicks_.y);
	ui.AxisYTickDirectionXSpin->setValue(axisTickDirection_[1].x);
	ui.AxisYTickDirectionYSpin->setValue(axisTickDirection_[1].y);
	ui.AxisYTickDirectionZSpin->setValue(axisTickDirection_[1].z);
	// -- Z
	ui.AxisZAutoTicksCheck->setChecked(axisAutoTicks_.z);
	ui.AxisZTicksStartSpin->setValue(axisFirstTick_.z);
	ui.AxisZTicksStartSpin->setEnabled(!axisAutoTicks_.z);
	ui.AxisZTicksDeltaSpin->setValue(axisTickDelta_.z);
	ui.AxisZTicksDeltaSpin->setEnabled(!axisAutoTicks_.z);
	ui.AxisZMinorTicksSpin->setValue(axisMinorTicks_.z);
	ui.AxisZTickDirectionXSpin->setValue(axisTickDirection_[2].x);
	ui.AxisZTickDirectionYSpin->setValue(axisTickDirection_[2].y);
	ui.AxisZTickDirectionZSpin->setValue(axisTickDirection_[2].z);

	// Text Orientation
	// -- X
	ui.AxisXLabelAxialRotationSlider->setValue(axisLabelOrientation_[0].x);
	ui.AxisXLabelAxialRotationSpin->setValue(axisLabelOrientation_[0].x);
	ui.AxisXLabelInPlaneRotationSlider->setValue(axisLabelOrientation_[0].y);
	ui.AxisXLabelInPlaneRotationSpin->setValue(axisLabelOrientation_[0].y);
	ui.AxisXLabelDistanceSpin->setValue(axisLabelOrientation_[0].z);
	ui.AxisYTitleHOffsetSlider->setValue(axisTitleOrientation_[0].w*1000);
	ui.AxisXTitleAnchorCombo->setCurrentIndex(axisTitleAnchor_[0]);
	ui.AxisXTitleAxialRotationSlider->setValue(axisTitleOrientation_[0].x);
	ui.AxisXTitleAxialRotationSpin->setValue(axisTitleOrientation_[0].x);
	ui.AxisXTitleInPlaneRotationSlider->setValue(axisTitleOrientation_[0].y);
	ui.AxisXTitleInPlaneRotationSpin->setValue(axisTitleOrientation_[0].y);
	ui.AxisXTitleDistanceSpin->setValue(axisTitleOrientation_[0].z);
	// -- Y
	ui.AxisYLabelAxialRotationSlider->setValue(axisLabelOrientation_[1].x);
	ui.AxisYLabelAxialRotationSpin->setValue(axisLabelOrientation_[1].x);
	ui.AxisYLabelInPlaneRotationSlider->setValue(axisLabelOrientation_[1].y);
	ui.AxisYLabelInPlaneRotationSpin->setValue(axisLabelOrientation_[1].y);
	ui.AxisYLabelDistanceSpin->setValue(axisLabelOrientation_[1].z);
	ui.AxisYTitleHOffsetSlider->setValue(axisTitleOrientation_[1].w*1000);
	ui.AxisYTitleAnchorCombo->setCurrentIndex(axisTitleAnchor_[1]);
	ui.AxisYTitleAxialRotationSlider->setValue(axisTitleOrientation_[1].x);
	ui.AxisYTitleAxialRotationSpin->setValue(axisTitleOrientation_[1].x);
	ui.AxisYTitleInPlaneRotationSlider->setValue(axisTitleOrientation_[1].y);
	ui.AxisYTitleInPlaneRotationSpin->setValue(axisTitleOrientation_[1].y);
	ui.AxisYTitleDistanceSpin->setValue(axisTitleOrientation_[1].z);
	// -- Z
	ui.AxisZLabelAxialRotationSlider->setValue(axisLabelOrientation_[2].x);
	ui.AxisZLabelAxialRotationSpin->setValue(axisLabelOrientation_[2].x);
	ui.AxisZLabelInPlaneRotationSlider->setValue(axisLabelOrientation_[2].y);
	ui.AxisZLabelInPlaneRotationSpin->setValue(axisLabelOrientation_[2].y);
	ui.AxisZLabelDistanceSpin->setValue(axisLabelOrientation_[2].z);
	ui.AxisZTitleHOffsetSlider->setValue(axisTitleOrientation_[2].w*1000);
	ui.AxisZTitleAnchorCombo->setCurrentIndex(axisTitleAnchor_[2]);
	ui.AxisZTitleAxialRotationSlider->setValue(axisTitleOrientation_[2].x);
	ui.AxisZTitleAxialRotationSpin->setValue(axisTitleOrientation_[2].x);
	ui.AxisZTitleInPlaneRotationSlider->setValue(axisTitleOrientation_[2].y);
	ui.AxisZTitleInPlaneRotationSpin->setValue(axisTitleOrientation_[2].y);
	ui.AxisZTitleDistanceSpin->setValue(axisTitleOrientation_[2].z);
	
	// Extras
	if (boundingBox_ == UChromaWindow::NoBox) ui.ViewBoundingBoxNoneRadio->setChecked(true);
	else if (boundingBox_ == UChromaWindow::PlaneBox) ui.ViewBoundingBoxPlaneRadio->setChecked(true);
	else if (boundingBox_ == UChromaWindow::CubeBox) ui.ViewBoundingBoxCubeRadio->setChecked(true);
	ui.ViewBoundingBoxPlaneYSpin->setValue(boundingBoxPlaneY_);

	// General Options
	ui.ViewLabelsFaceViewerCheck->setChecked(labelFaceViewer_);

	refreshing_ = false;
}
